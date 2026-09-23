#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 这一版直接按公开 STC89C52RC 双红外循迹例程的结构写：
 *
 *   00 -> 直行
 *   10 -> 左转
 *   01 -> 右转
 *   11 -> 不改变当前电机命令
 *
 * 没有定时保持、没有“上次方向”状态机、没有 2 s delay。
 * 传感器每次状态变化，while(1) 下一轮立即重新决定动作。
 */

static void drive_straight(void)
{
    motor_set(SPEED_STRAIGHT_LEFT, SPEED_STRAIGHT_RIGHT);
}

static void turn_left(void)
{
    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
}

static void turn_right(void)
{
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

    /*
     * 00：两个灯都亮，两个探头都在白底 -> 直行。
     */
    if (pattern == TRACK_PATTERN_BOTH_WHITE)
    {
        drive_straight();
    }
    /*
     * 10：左灯灭、右灯亮 -> 左边碰到黑线 -> 左转。
     */
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        turn_left();
    }
    /*
     * 01：左灯亮、右灯灭 -> 右边碰到黑线 -> 右转。
     */
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        turn_right();
    }
    /*
     * 11：两个灯都灭。
     *
     * 两个数字传感器无法从 11 判断应该左转还是右转。
     * 参考简单开源例程的处理方式，这里不覆盖当前电机命令：
     * - 如果刚才已经在左/右转，就继续该动作；
     * - 如果刚才在直行，就继续直行；
     * - 如果刚上电还没得到有效状态，则保持停止。
     *
     * 一旦重新出现 00/10/01，下一轮循环立即更新动作。
     */
    else
    {
        /* keep previous motor command */
    }
}
