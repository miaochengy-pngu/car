#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 直接按上传开源例程的循迹判断改写：
 *
 *   HWSM_R==0 && HWSM_L==0 -> 直行
 *   HWSM_R==1 && HWSM_L==0 -> 右转
 *   HWSM_L==1 && HWSM_R==0 -> 左转
 *   else                    -> 直行
 *
 * 对应你的实车：
 *   灯亮 = 0
 *   灯灭 = 1
 *
 * 因此：
 *   00 两灯亮 -> 直行
 *   01 右灯灭 -> 右转
 *   10 左灯灭 -> 左转
 *   11 两灯灭 -> 直行
 *
 * 不加 PID、不加延时、不加历史方向、不加状态机。
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

    if (pattern == TRACK_PATTERN_BOTH_WHITE)
    {
        /* 00：两灯亮，黑线位于两个探头之间 */
        run_forward();
    }
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        /* 01：右灯灭 -> 右转 */
        turn_right();
    }
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        /* 10：左灯灭 -> 左转 */
        turn_left();
    }
    else
    {
        /*
         * 11：两灯都灭。
         * 上传例程这里直接保持正转，因此这里也直接直行。
         */
        run_forward();
    }
}
