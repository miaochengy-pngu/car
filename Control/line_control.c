#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

/*
 * FSM:
 *
 * FOLLOW_LINE
 *      |
 *      | outer sensor detects black
 *      v
 * TURN_LEFT / TURN_RIGHT
 *      |
 *      | keep differential steering for TURN_TIME_MS cycles
 *      v
 * FOLLOW_LINE
 *
 * Sensor:
 * black line = 1
 * white = 0
 */

typedef enum
{
    FOLLOW_LINE = 0,
    TURN_LEFT_STATE,
    TURN_RIGHT_STATE
} CarState;

static CarState state = FOLLOW_LINE;
static unsigned int turn_counter = 0;

static void run_forward(void)
{
    motor_set(RUN_LEFT_SPEED, RUN_RIGHT_SPEED);
}

static void turn_left(void)
{
    /* left wheel reverse, right wheel forward */
    motor_set(-100, 100);
}

static void turn_right(void)
{
    /* left wheel forward, right wheel reverse */
    motor_set(100, -100);
}

static void follow_line(void)
{
    unsigned char pattern;

    pattern = tracking_read_pattern();

    if(pattern == TRACK_PATTERN_BOTH_BLACK)
    {
        run_forward();
    }
    else if(pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        motor_set(-60, 100);
    }
    else if(pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        motor_set(100, -60);
    }
    else
    {
        run_forward();
    }
}

void line_control_init(void)
{
    state = FOLLOW_LINE;
    turn_counter = 0;
    motor_stop();
}

void line_control_step(void)
{
    switch(state)
    {
        case FOLLOW_LINE:

            if(tracking_outer_left())
            {
                state = TURN_LEFT_STATE;
                turn_counter = 0;
                break;
            }

            if(tracking_outer_right())
            {
                state = TURN_RIGHT_STATE;
                turn_counter = 0;
                break;
            }

            follow_line();
            break;


        case TURN_LEFT_STATE:

            /* keep turning, ignore sensors */
            turn_left();
            turn_counter++;

            if(turn_counter >= TURN_TIME_MS)
            {
                state = FOLLOW_LINE;
                turn_counter = 0;
            }
            break;


        case TURN_RIGHT_STATE:

            /* keep turning, ignore sensors */
            turn_right();
            turn_counter++;

            if(turn_counter >= TURN_TIME_MS)
            {
                state = FOLLOW_LINE;
                turn_counter = 0;
            }
            break;
    }
}
