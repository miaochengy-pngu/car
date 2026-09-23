#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

#define LAST_NONE   0
#define LAST_LEFT   1
#define LAST_RIGHT  2

static unsigned char g_last_turn = LAST_NONE;

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
    g_last_turn = LAST_NONE;
    motor_stop();
}

void line_control_step(void)
{
    unsigned char pattern;

    pattern = tracking_read_pattern();

    /*
     * 当前实车：
     *   11 = 两灯灭 = 黑线位于正常循迹位置
     *   10 = 左侧检测黑线
     *   01 = 右侧检测黑线
     *   00 = 两侧都没有检测到黑线
     */

    if (pattern == TRACK_PATTERN_BOTH_BLACK)
    {
        /* 11：正常居中，直行 */
        run_forward();
    }
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        /* 10：向左修正 */
        g_last_turn = LAST_LEFT;
        turn_left();
    }
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        /* 01：向右修正 */
        g_last_turn = LAST_RIGHT;
        turn_right();
    }
    else
    {
        /*
         * 00：丢线。
         * 按刚才的修正方向继续找线，不增加延时或状态机。
         */
        if (g_last_turn == LAST_LEFT)
        {
            turn_left();
        }
        else if (g_last_turn == LAST_RIGHT)
        {
            turn_right();
        }
        else
        {
            /* 上电后还没有方向信息时先停住 */
            motor_stop();
        }
    }
}
