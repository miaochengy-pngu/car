#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "delay.h"

/*
 * 四路红外策略：
 * 外侧传感器 -> 90度转弯触发
 * 内侧传感器 -> 普通循迹
 */

static void run_forward(void)
{
    motor_set(RUN_LEFT_SPEED, RUN_RIGHT_SPEED);
}

static void turn_left(void)
{
    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
}

static void turn_right(void)
{
    motor_set(TURN_OUTER_SPEED, TURN_INNER_SPEED);
}

static void follow_line(void)
{
    unsigned char pattern;
    pattern = tracking_read_pattern();

    if ((pattern == TRACK_PATTERN_BOTH_WHITE) ||
        (pattern == TRACK_PATTERN_BOTH_BLACK))
    {
        run_forward();
    }
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        turn_left();
    }
    else
    {
        turn_right();
    }
}

void line_control_init(void)
{
    motor_stop();
}

void line_control_step(void)
{
    /* 外侧传感器优先处理90度弯 */
    if (tracking_outer_left())
    {
        turn_left();
        delay_ms(TURN_TIME_MS);
        return;
    }

    if (tracking_outer_right())
    {
        turn_right();
        delay_ms(TURN_TIME_MS);
        return;
    }

    follow_line();
}
