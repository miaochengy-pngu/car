#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * direction:
 *   -1 = 最近一次需要向左修正
 *    0 = 尚无历史方向
 *   +1 = 最近一次需要向右修正
 */
static signed char g_last_direction = 0;

/* 连续保持同一偏离方向的控制周期数 */
static unsigned char g_deviation_count = 0;

static void drive_left(unsigned char strong)
{
    if (strong)
    {
        motor_set(TURN_HARD_INNER, TURN_HARD_OUTER);
    }
    else
    {
        motor_set(TURN_SOFT_INNER, TURN_SOFT_OUTER);
    }
}

static void drive_right(unsigned char strong)
{
    if (strong)
    {
        motor_set(TURN_HARD_OUTER, TURN_HARD_INNER);
    }
    else
    {
        motor_set(TURN_SOFT_OUTER, TURN_SOFT_INNER);
    }
}

static void update_deviation(signed char direction)
{
    if (g_last_direction == direction)
    {
        if (g_deviation_count < 255)
        {
            g_deviation_count++;
        }
    }
    else
    {
        g_last_direction = direction;
        g_deviation_count = 1;
    }
}

static void recover_line(void)
{
    /*
     * 不做复杂状态机。
     * 完全丢线时只按照最近一次修正方向继续低速找线。
     */
    g_deviation_count = 0;

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
    g_deviation_count = 0;
    motor_stop();
}

void line_control_step(void)
{
    unsigned char pattern;
    unsigned char opposite_pattern;
    unsigned char strong_turn;

    pattern = tracking_read_pattern();

#if TRACK_CENTER_PATTERN == 3
    opposite_pattern = TRACK_PATTERN_BOTH_WHITE;
#else
    opposite_pattern = TRACK_PATTERN_BOTH_BLACK;
#endif

    /* 正常居中：高速直行，并清空“持续偏离”计数。 */
    if (pattern == TRACK_CENTER_PATTERN)
    {
        g_deviation_count = 0;
        motor_set(SPEED_STRAIGHT, SPEED_STRAIGHT);
        return;
    }

    /* 完全丢线/特殊状态：按上一次方向低速找回。 */
    if (pattern == opposite_pattern)
    {
        recover_line();
        return;
    }

    /*
     * 10：黑线更靠左 -> 小车向左修正
     * 01：黑线更靠右 -> 小车向右修正
     */
    if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        update_deviation(-1);
        strong_turn = (g_deviation_count >= HARD_TURN_COUNT) ? 1 : 0;
        drive_left(strong_turn);
        return;
    }

    if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        update_deviation(1);
        strong_turn = (g_deviation_count >= HARD_TURN_COUNT) ? 1 : 0;
        drive_right(strong_turn);
        return;
    }

    motor_stop();
}
