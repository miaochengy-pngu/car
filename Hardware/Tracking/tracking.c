#include <REG52.H>
#include "config.h"
#include "delay.h"
#include "tracking.h"

/*
 * Dedicated tracking connector on the user's VER:2.3 board:
 *   OTL -> P3.5
 *   OTR -> P3.4
 */
sbit TRACK_LEFT  = P3^5;   /* OTL */
sbit TRACK_RIGHT = P3^4;   /* OTR */

static unsigned char g_center_pattern = TRACK_CENTER_DEFAULT;

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
     * 8051 quasi-bidirectional inputs are released by writing 1.
     */
    TRACK_LEFT = 1;
    TRACK_RIGHT = 1;

    g_center_pattern = TRACK_CENTER_DEFAULT;
}

unsigned char tracking_read_pattern(void)
{
    unsigned char left_black;
    unsigned char right_black;

    left_black = is_black(TRACK_LEFT);
    right_black = is_black(TRACK_RIGHT);

    return (unsigned char)((left_black << 1) | right_black);
}

void tracking_calibrate_center(void)
{
#if TRACK_AUTO_CENTER_ENABLE
    unsigned char counts[4];
    unsigned char i;
    unsigned char pattern;
    unsigned char best_pattern;
    unsigned char best_count;

    counts[0] = 0;
    counts[1] = 0;
    counts[2] = 0;
    counts[3] = 0;

    /*
     * Place the car centered on the track before power-on.
     * 64 samples over about 128 ms reject comparator chatter.
     */
    for (i = 0; i < 64; i++)
    {
        pattern = tracking_read_pattern();
        counts[pattern]++;
        delay_ms(2);
    }

    best_pattern = 0;
    best_count = counts[0];

    for (i = 1; i < 4; i++)
    {
        if (counts[i] > best_count)
        {
            best_count = counts[i];
            best_pattern = i;
        }
    }

    /*
     * A sensible centered two-sensor geometry is normally 00
     * (line between sensors) or 11 (both sensors over a wide line).
     * If startup was misaligned, fall back to the configured default.
     */
    if ((best_pattern == TRACK_PATTERN_BOTH_WHITE) ||
        (best_pattern == TRACK_PATTERN_BOTH_BLACK))
    {
        g_center_pattern = best_pattern;
    }
    else
    {
        g_center_pattern = TRACK_CENTER_DEFAULT;
    }
#endif
}

unsigned char tracking_get_center_pattern(void)
{
    return g_center_pattern;
}
