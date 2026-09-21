#include <REG52.H>
#include "config.h"
#include "motor.h"
#include "tracking.h"

/*
 * The dedicated tracking connector on the user's VER:2.3 board is:
 *   OTL -> P3.5
 *   OTR -> P3.4
 */
sbit TRACK_LEFT  = P3^5;   /* OTL */
sbit TRACK_RIGHT = P3^4;   /* OTR */

static unsigned char is_black(unsigned char level)
{
#if TRACK_BLACK_LEVEL == 1
    return level ? 1 : 0;
#else
    return level ? 0 : 1;
#endif
}

void tracking_init(void)
{
    /*
     * 8051 quasi-bidirectional ports are released for input by writing 1.
     */
    TRACK_LEFT  = 1;
    TRACK_RIGHT = 1;
}

void tracking_control(void)
{
    unsigned char left_black;
    unsigned char right_black;

    left_black  = is_black(TRACK_LEFT);
    right_black = is_black(TRACK_RIGHT);

    /*
     * 0 0: both sensors on white -> go straight
     * 1 0: left sensor reaches black -> correct left
     * 0 1: right sensor reaches black -> correct right
     * 1 1: both on black -> stop (finish-line policy)
     */
    if ((left_black == 0) && (right_black == 0))
    {
        car_forward();
    }
    else if ((left_black == 1) && (right_black == 0))
    {
        car_turn_left();
    }
    else if ((left_black == 0) && (right_black == 1))
    {
        car_turn_right();
    }
    else
    {
#if STOP_ON_BOTH_BLACK
        car_stop();
#else
        car_forward();
#endif
    }
}
