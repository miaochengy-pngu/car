#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * 四路红外循迹：
 *
 * 外侧传感器：90度弯触发
 *   检测黑线(1) -> 进入固定转弯状态
 *
 * 内侧传感器：普通循迹
 *
 * 黑线 = 1
 * 白底 = 0
 */

typedef enum
{
    FOLLOW_LINE = 0,
    TURN_LEFT_STATE,
    TURN_RIGHT_STATE
} CarState;

static CarState state = FOLLOW_LINE;
static unsigned int turn_count = 0;

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
}

void line_control_step(void)
{
    switch(state)
    {
        case FOLLOW_LINE:

            /* 外侧传感器优先触发90度转弯 */
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
                state = FOLLOW_LINE;
            }
            break;


        case TURN_RIGHT_STATE:

            turn_right();
            turn_count++;

            if (turn_count >= TURN_TIME_MS)
            {
                state = FOLLOW_LINE;
            }
            break;
    }
}
