#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "timer.h"

typedef enum
{
    FOLLOW_LINE = 0,
    TURN_LEFT_STATE,
    TURN_RIGHT_STATE,
    OUTER_LOCKOUT_STATE
} CarState;

static CarState state = FOLLOW_LINE;
static unsigned int state_start_time = 0;

static unsigned char prev_outer_left = 0;
static unsigned char prev_outer_right = 0;

static unsigned char left_error_count = 0;
static unsigned char right_error_count = 0;

static unsigned int elapsed_ms(unsigned int now, unsigned int start)
{
    return (unsigned int)(now - start);
}

static void run_forward(void)
{
    motor_set(RUN_LEFT_SPEED, RUN_RIGHT_SPEED);
}

static void track_left(void)
{
    motor_set(TRACK_INNER_SPEED, TRACK_OUTER_SPEED);
}

static void track_right(void)
{
    motor_set(TRACK_OUTER_SPEED, TRACK_INNER_SPEED);
}

static void corner_left(void)
{
    motor_set(CORNER_INNER_SPEED, CORNER_OUTER_SPEED);
}

static void corner_right(void)
{
    motor_set(CORNER_OUTER_SPEED, CORNER_INNER_SPEED);
}

static void follow_line(void)
{
    unsigned char pattern;

    pattern = tracking_read_pattern();

    if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        if (left_error_count < TRACK_FILTER_COUNT)
            left_error_count++;
        right_error_count = 0;

        if (left_error_count >= TRACK_FILTER_COUNT)
            track_left();
        else
            run_forward();
    }
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        if (right_error_count < TRACK_FILTER_COUNT)
            right_error_count++;
        left_error_count = 0;

        if (right_error_count >= TRACK_FILTER_COUNT)
            track_right();
        else
            run_forward();
    }
    else
    {
        left_error_count = 0;
        right_error_count = 0;
        run_forward();
    }
}

void line_control_init(void)
{
    motor_stop();

    state = FOLLOW_LINE;
    state_start_time = timer1_get_ms();

    prev_outer_left = tracking_outer_left();
    prev_outer_right = tracking_outer_right();
}

void line_control_step(void)
{
    unsigned int now;
    unsigned char outer_left;
    unsigned char outer_right;

    now = timer1_get_ms();

    switch (state)
    {
        case FOLLOW_LINE:
            follow_line();

            outer_left = tracking_outer_left();
            outer_right = tracking_outer_right();

            if ((prev_outer_left == 0) && (prev_outer_right == 0))
            {
                if ((outer_left == 0) && (outer_right == 1))
                {
                    state = TURN_RIGHT_STATE;
                    state_start_time = now;
                    corner_right();
                }
                else if ((outer_left == 1) && (outer_right == 0))
                {
                    state = TURN_LEFT_STATE;
                    state_start_time = now;
                    corner_left();
                }
            }

            prev_outer_left = outer_left;
            prev_outer_right = outer_right;
            break;

        case TURN_LEFT_STATE:
            corner_left();
            if (elapsed_ms(now, state_start_time) >= TURN_DURATION_MS)
            {
                state = OUTER_LOCKOUT_STATE;
                state_start_time = now;
            }
            break;

        case TURN_RIGHT_STATE:
            corner_right();
            if (elapsed_ms(now, state_start_time) >= TURN_DURATION_MS)
            {
                state = OUTER_LOCKOUT_STATE;
                state_start_time = now;
            }
            break;

        case OUTER_LOCKOUT_STATE:
            follow_line();

            if (elapsed_ms(now, state_start_time) >= OUTER_LOCKOUT_MS)
            {
                prev_outer_left = tracking_outer_left();
                prev_outer_right = tracking_outer_right();
                state = FOLLOW_LINE;
            }
            break;

        default:
            state = FOLLOW_LINE;
            break;
    }
}
