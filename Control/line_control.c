#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 和常见 STC89C52RC 双红外循迹例程一样：
 * 主循环不断读传感器，直接更新左右轮目标 PWM。
 *
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
     * 居中：左右同速。
     */
    if (pattern == TRACK_CENTER_PATTERN)
    {
        motor_set(SPEED_STRAIGHT, SPEED_STRAIGHT);
        return;
    }

    /*
     * 左探头碰到黑线：左轮减速、右轮加速。
     */
    if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        turn_left();
        return;
    }

    /*
     * 右探头碰到黑线：右轮减速、左轮加速。
     */
    if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        turn_right();
        return;
    }

    /*
     * 两个探头进入与“居中”相反的状态：
     * 对 90°直角，可能是拐角处短暂全黑/全白。
     * 按刚才的转向方向继续找线。
     */
    if (pattern == opposite_pattern)
    {
        recover_line();
        return;
    }

    motor_stop();
}
