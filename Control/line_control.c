#include "config.h"
#include "pid.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

static PID_Controller g_steering_pid;

static int g_last_nonzero_error = 0;
static unsigned char g_memory_ticks = 0;

static int abs_int(int value)
{
    return (value < 0) ? -value : value;
}

static int clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value)
    {
        return min_value;
    }

    if (value > max_value)
    {
        return max_value;
    }

    return value;
}

static int error_from_pattern(unsigned char pattern,
                              unsigned char center_pattern)
{
    /*
     * Two useful mounting geometries exist:
     *
     * center = 00:
     *   line is between the two sensors
     *   10 -> steer left
     *   01 -> steer right
     *
     * center = 11:
     *   both sensors are over a wide black line
     *   01 -> steer left
     *   10 -> steer right
     */
    if (center_pattern == TRACK_PATTERN_BOTH_BLACK)
    {
        if (pattern == TRACK_PATTERN_RIGHT_BLACK)
        {
            return -TRACK_ERROR_FULL;
        }

        if (pattern == TRACK_PATTERN_LEFT_BLACK)
        {
            return TRACK_ERROR_FULL;
        }
    }
    else
    {
        if (pattern == TRACK_PATTERN_LEFT_BLACK)
        {
            return -TRACK_ERROR_FULL;
        }

        if (pattern == TRACK_PATTERN_RIGHT_BLACK)
        {
            return TRACK_ERROR_FULL;
        }
    }

    return 0;
}

void line_control_init(void)
{
    pid_init(&g_steering_pid,
             STEER_KP_X100,
             STEER_KI_X100,
             STEER_KD_X100,
             STEER_INTEGRAL_LIMIT,
             STEER_OUTPUT_LIMIT);

    g_last_nonzero_error = 0;
    g_memory_ticks = 0;

    motor_stop();
}

void line_control_step(void)
{
    unsigned char pattern;
    unsigned char center_pattern;

    int error;
    int correction;
    int base_speed;
    int slowdown;

    int left_command;
    int right_command;

    pattern = tracking_read_pattern();
    center_pattern = tracking_get_center_pattern();

    if (pattern == center_pattern)
    {
        /*
         * When the two-sensor car returns to its nominal pattern,
         * decay the previous steering error instead of snapping
         * immediately to zero. This is the digital-sensor analogue
         * of the "momentum" / line-loss memory used by faster
         * open-source line followers.
         */
        if (g_memory_ticks > 0)
        {
            error = (g_last_nonzero_error * (int)g_memory_ticks)
                  / STEERING_MEMORY_TICKS;

            g_memory_ticks--;
        }
        else
        {
            error = 0;
        }
    }
    else if ((pattern == TRACK_PATTERN_LEFT_BLACK) ||
             (pattern == TRACK_PATTERN_RIGHT_BLACK))
    {
        error = error_from_pattern(pattern, center_pattern);

        g_last_nonzero_error = error;
        g_memory_ticks = STEERING_MEMORY_TICKS;
    }
    else
    {
        /*
         * Opposite of the calibrated center state:
         *
         * center=11 and pattern=00 -> definite line loss.
         * center=00 and pattern=11 -> broad black area/intersection.
         *
         * In either case, keep the previous turn direction instead
         * of blindly driving straight.
         */
        pid_reset(&g_steering_pid);

        if (g_last_nonzero_error < 0)
        {
            motor_set(-SEARCH_REVERSE_SPEED, SEARCH_FORWARD_SPEED);
        }
        else if (g_last_nonzero_error > 0)
        {
            motor_set(SEARCH_FORWARD_SPEED, -SEARCH_REVERSE_SPEED);
        }
        else
        {
            motor_set(SPECIAL_STRAIGHT_SPEED, SPECIAL_STRAIGHT_SPEED);
        }

        return;
    }

    correction = pid_update(&g_steering_pid, error);

    /*
     * Adaptive speed:
     *   - straight sections use high base PWM
     *   - larger steering demand automatically reduces base speed
     *   - this gives higher top speed without entering corners at
     *     the same speed as a straight.
     */
    slowdown = (abs_int(correction) * SPEED_SLOWDOWN_MAX)
             / STEER_OUTPUT_LIMIT;

    base_speed = SPEED_STRAIGHT - slowdown;
    base_speed = clamp_int(base_speed, SPEED_MIN, SPEED_MAX);

    left_command = base_speed + correction;
    right_command = base_speed - correction;

    left_command = clamp_int(left_command, -100, 100);
    right_command = clamp_int(right_command, -100, 100);

    motor_set(left_command, right_command);
}
