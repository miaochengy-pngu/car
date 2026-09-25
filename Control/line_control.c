#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "delay.h"

/*
 * 四路红外循迹：
 *
 * 外侧传感器：只负责90度转弯
 *   外左检测黑线 -> 左转1秒
 *   外右检测黑线 -> 右转1秒
 *
 * 内侧传感器：负责普通循迹
 *   黑线 = 1
 *   白底 = 0
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

    /*
     * 内侧循迹：
     * 11: 黑线位于中心 -> 直行
     * 10: 左侧偏移 -> 左修正
     * 01: 右侧偏移 -> 右修正
     */

    if (pattern == TRACK_PATTERN_BOTH_BLACK)
    {
        run_forward();
    }
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        turn_left();
    }
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        turn_right();
    }
    else
    {
        run_forward();
    }
}

void line_control_init(void)
{
    motor_stop();
}

void line_control_step(void)
{
    /* 外侧传感器优先：检测90度弯 */
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

    /* 没有大弯，执行普通循迹 */
    follow_line();
}
