#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 最简单的双数字红外循迹逻辑：
 *
 *   两个传感器相同（00 或 11） -> 直行
 *   10 -> 左转
 *   01 -> 右转
 *
 * 这样不依赖指示灯极性来决定“中心到底是 00 还是 11”。
 * 只要左右不一致，就按哪一侧检测到黑线来修正。
 *
 * 不使用 PID、延时、状态机或历史方向。
 */

static void run_forward(void)
{
    motor_set(RUN_LEFT_SPEED, RUN_RIGHT_SPEED);
}

static void turn_left(void)
{
    /* 左轮反转，右轮正转 */
    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
}

static void turn_right(void)
{
    /* 左轮正转，右轮反转 */
    motor_set(TURN_OUTER_SPEED, TURN_INNER_SPEED);
}

void line_control_init(void)
{
    motor_stop();
}

void line_control_step(void)
{
    unsigned char pattern;

    pattern = tracking_read_pattern();

    if ((pattern == TRACK_PATTERN_BOTH_WHITE) ||
        (pattern == TRACK_PATTERN_BOTH_BLACK))
    {
        /* 00 或 11：左右状态相同，继续向前 */
        run_forward();
    }
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        /* 10：左侧检测到黑线 -> 左转 */
        turn_left();
    }
    else
    {
        /* 01：右侧检测到黑线 -> 右转 */
        turn_right();
    }
}
