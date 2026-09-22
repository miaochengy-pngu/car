#include "config.h"
#include "pid.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

static PID_Controller g_pd;
static int g_last_error = 0;

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

/*
 * 两路数字红外只提供三档有效误差：
 *
 *   10 -> 黑线偏左 -> 小车向左修正 -> error = -100
 *   居中状态        -> error = 0
 *   01 -> 黑线偏右 -> 小车向右修正 -> error = +100
 */
static int pattern_to_error(unsigned char pattern)
{
    if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        return -TRACK_ERROR;
    }

    if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        return TRACK_ERROR;
    }

    return 0;
}

static void recover_line(void)
{
    /*
     * 特殊状态 = 与 TRACK_CENTER_PATTERN 相反的全白/全黑状态。
     * 不做复杂状态机，只按上一次偏移方向低速找线。
     */
    pid_reset(&g_pd);

    if (g_last_error < 0)
    {
        /* 向左找：左轮慢、右轮快 */
        motor_set(RECOVER_INNER_SPEED, RECOVER_OUTER_SPEED);
    }
    else if (g_last_error > 0)
    {
        /* 向右找：左轮快、右轮慢 */
        motor_set(RECOVER_OUTER_SPEED, RECOVER_INNER_SPEED);
    }
    else
    {
        motor_stop();
    }
}

void line_control_init(void)
{
    pid_init(&g_pd,
             STEER_KP_X100,
             STEER_KI_X100,
             STEER_KD_X100,
             STEER_INTEGRAL_LIMIT,
             STEER_OUTPUT_LIMIT);

    g_last_error = 0;
    motor_stop();
}

void line_control_step(void)
{
    unsigned char pattern;
    unsigned char opposite_pattern;

    int error;
    int correction;
    int base_speed;
    int left_speed;
    int right_speed;

    pattern = tracking_read_pattern();

#if TRACK_CENTER_PATTERN == 3
    opposite_pattern = TRACK_PATTERN_BOTH_WHITE;
#else
    opposite_pattern = TRACK_PATTERN_BOTH_BLACK;
#endif

    if (pattern == opposite_pattern)
    {
        recover_line();
        return;
    }

    if (pattern == TRACK_CENTER_PATTERN)
    {
        error = 0;
        base_speed = SPEED_STRAIGHT;
    }
    else
    {
        error = pattern_to_error(pattern);
        base_speed = SPEED_TURN;

        if (error != 0)
        {
            g_last_error = error;
        }
    }

    correction = pid_update(&g_pd, error);

    /*
     * correction > 0：向右修正
     * correction < 0：向左修正
     */
    left_speed  = base_speed + correction;
    right_speed = base_speed - correction;

    left_speed  = clamp_int(left_speed, 0, 100);
    right_speed = clamp_int(right_speed, 0, 100);

    motor_set(left_speed, right_speed);
}
