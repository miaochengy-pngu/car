#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 四路红外循迹状态机：
 *
 * FOLLOW_LINE
 *      |
 *      | 外侧传感器检测黑线
 *      v
 * TURN_LEFT_STATE / TURN_RIGHT_STATE
 *      |
 *      | 持续输出差速转弯
 *      v
 * COOLDOWN_STATE
 *      |
 *      | 1s内禁止再次判断外侧转弯传感器
 *      v
 * FOLLOW_LINE
 *
 * 黑线 = 1 (灯灭)
 */

typedef enum
{
    FOLLOW_LINE = 0,
    TURN_LEFT_STATE,
    TURN_RIGHT_STATE,
    TURN_COOLDOWN_STATE
} CarState;

static CarState state = FOLLOW_LINE;
static unsigned int turn_count = 0;
static unsigned int cooldown_count = 0;

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
    state = FOLLOW_LINE;
    turn_count = 0;
    cooldown_count = 0;
}

void line_control_step(void)
{
    switch(state)
    {
        case FOLLOW_LINE:

            if (tracking_outer_left())
            {
                state = TURN_LEFT_STATE;
                turn_count = 0;
            }
            else if (tracking_outer_right())
            {
                state = TURN_RIGHT_STATE;
                turn_count = 0;
            }
            else
            {
                follow_line();
            }
            break;

        case TURN_LEFT_STATE:

            turn_left();
            turn_count++;

            if (turn_count >= TURN_TIME_MS)
            {
                turn_count = 0;
                cooldown_count = 0;
                state = TURN_COOLDOWN_STATE;
            }
            break;

        case TURN_RIGHT_STATE:

            turn_right();
            turn_count++;

            if (turn_count >= TURN_TIME_MS)
            {
                turn_count = 0;
                cooldown_count = 0;
                state = TURN_COOLDOWN_STATE;
            }
            break;

        case TURN_COOLDOWN_STATE:

            /* 转弯结束后继续前进，1秒内忽略外侧传感器 */
            run_forward();
            cooldown_count++;

            if (cooldown_count >= TURN_TIME_MS)
            {
                cooldown_count = 0;
                state = FOLLOW_LINE;
            }
            break;
    }
}
