#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 这里直接照公开 STC89C52RC 双红外循迹例程的控制结构改写。
 *
 * 原例程 main.c 的核心就是：
 *
 *   00 -> run()
 *   10 -> leftrun()
 *   01 -> rightrun()
 *
 * leftrun():
 *   左轮后退，右轮前进
 *
 * rightrun():
 *   左轮前进，右轮后退
 *
 * 原例程没有 PID、没有滤波、没有延时锁定、没有状态机。
 * 11 时也没有新的电机赋值，因此自然保持上一条命令。
 */

static void run_forward(void)
{
    motor_set(RUN_LEFT_SPEED, RUN_RIGHT_SPEED);
}

static void left_run(void)
{
    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
}

static void right_run(void)
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

    /* 两灯亮：黑线在两个探头之间，直行 */
    if (pattern == TRACK_PATTERN_BOTH_WHITE)
    {
        run_forward();
    }
    /* 左灯灭、右灯亮：左边碰到黑线，原地左转 */
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        left_run();
    }
    /* 左灯亮、右灯灭：右边碰到黑线，原地右转 */
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        right_run();
    }
    else
    {
        /*
         * 11：和所参考的开源“循迹小车实验3”一致，
         * 不发送新的电机命令，保持上一动作。
         */
    }
}
