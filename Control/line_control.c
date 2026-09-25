#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "timer.h"

/*
 * 状态机:
 * FOLLOW_LINE
 *      |
 *      | 外侧传感器触发
 *      v
 * TURN_LEFT / TURN_RIGHT
 *      |
 *      | Timer0计时
 *      v
 * FOLLOW_LINE
 */

typedef enum
{
    FOLLOW_LINE = 0,
    TURN_LEFT_STATE,
    TURN_RIGHT_STATE
} CarState;

static CarState state = FOLLOW_LINE;
static unsigned int state_start_time = 0;

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
    state_start_time = 0;
}

void line_control_step(void)
{
    unsigned int now;
    now = timer0_get_ms();

    switch(state)
    {
        case FOLLOW_LINE:

            if (tracking_outer_left())
            {
                state = TURN_LEFT_STATE;
                state_start_time = now;
            }
            else if (tracking_outer_right())
            {
                state = TURN_RIGHT_STATE;
                state_start_time = now;
            }
            else
            {
                follow_line();
            }
            break;

        case TURN_LEFT_STATE:

            turn_left();

            if ((unsigned int)(now - state_start_time) >= TURN_DURATION_MS)
            {
                state = FOLLOW_LINE;
            }
            break;

        case TURN_RIGHT_STATE:

            turn_right();

            if ((unsigned int)(now - state_start_time) >= TURN_DURATION_MS)
            {
                state = FOLLOW_LINE;
            }
            break;
    }
}
