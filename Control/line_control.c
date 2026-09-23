#include "config.h"
#include "delay.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 触发式转弯：
 *
 * 1. 正常 00 时直行，并允许下一次转弯触发；
 * 2. 首次检测到 10/01 时，只触发一次固定时长转弯；
 * 3. 固定转弯结束后强制回到直行；
 * 4. 必须重新看到 00 后，才允许下一次触发。
 *
 * 这样不会再因为“上一次是左转”而在拿起/放下后一直左转。
 */
static unsigned char g_turn_armed = 1;

static void drive_straight(void)
{
    motor_set(SPEED_STRAIGHT_LEFT, SPEED_STRAIGHT_RIGHT);
}

static void trigger_left_turn(void)
{
    g_turn_armed = 0;

    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
    delay_ms(TURN_HOLD_MS);

    drive_straight();
}

static void trigger_right_turn(void)
{
    g_turn_armed = 0;

    motor_set(TURN_OUTER_SPEED, TURN_INNER_SPEED);
    delay_ms(TURN_HOLD_MS);

    drive_straight();
}

void line_control_init(void)
{
    g_turn_armed = 1;
    motor_stop();
}

void line_control_step(void)
{
    unsigned char pattern;

    pattern = tracking_read_pattern();

    /*
     * 00：正常直行。
     * 同时重新武装，下一次 10/01 才能再次触发转弯。
     */
    if (pattern == TRACK_CENTER_PATTERN)
    {
        g_turn_armed = 1;
        drive_straight();
        return;
    }

    /*
     * 一次转弯结束以后，在重新见到 00 之前不允许重复触发。
     * 此时保持直行，而不是继续沿旧方向转。
     */
    if (!g_turn_armed)
    {
        drive_straight();
        return;
    }

    /*
     * 10：左边检测到黑线 -> 触发一次左转。
     */
    if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        trigger_left_turn();
        return;
    }

    /*
     * 01：右边检测到黑线 -> 触发一次右转。
     */
    if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        trigger_right_turn();
        return;
    }

    /*
     * 11：两个探头同时黑，方向信息本身是歧义的。
     * 上电悬空也可能表现为两灯灭，因此这里不再沿用旧方向，
     * 直接停车，避免拿起/放下后持续原地转。
     */
    motor_stop();
}
