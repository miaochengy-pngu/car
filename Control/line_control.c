#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * last_direction:
 *   -1 = 上一次向左修正
 *    0 = 尚未出现偏离
 *   +1 = 上一次向右修正
 */
static signed char g_last_direction = 0;

static void turn_left(void)
{
    g_last_direction = -1;
    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
}

static void turn_right(void)
{
    g_last_direction = 1;
    motor_set(TURN_OUTER_SPEED, TURN_INNER_SPEED);
}

static void recover_line(void)
{
    if (g_last_direction < 0)
    {
        motor_set(RECOVER_INNER_SPEED, RECOVER_OUTER_SPEED);
    }
    else if (g_last_direction > 0)
    {
        motor_set(RECOVER_OUTER_SPEED, RECOVER_INNER_SPEED);
    }
    else
    {
        motor_stop();
    }
}

void line_control_init(void)
{
    g_last_direction = 0;
    motor_stop();
}

void line_control_step(void)
{
    unsigned char pattern;
    unsigned char opposite_pattern;

    pattern = tracking_read_pattern();

#if TRACK_CENTER_PATTERN == 0
    opposite_pattern = TRACK_PATTERN_BOTH_BLACK;
#else
    opposite_pattern = TRACK_PATTERN_BOTH_WHITE;
#endif

    /*
     * 居中：左右轮使用独立的基础 PWM。
     * 这是没有编码器时补偿左右电机差异的最直接办法。
     */
    if (pattern == TRACK_CENTER_PATTERN)
    {
        motor_set(SPEED_STRAIGHT_LEFT, SPEED_STRAIGHT_RIGHT);
        return;
    }

    if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        turn_left();
        return;
    }

    if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        turn_right();
        return;
    }

    if (pattern == opposite_pattern)
    {
        recover_line();
        return;
    }

    motor_stop();
}
