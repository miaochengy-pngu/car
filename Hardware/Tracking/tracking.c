#include <REG52.H>
#include "tracking.h"

/*
 * 四路红外：
 *
 * 内侧：普通循迹
 *   LEFT  -> P3.5
 *   RIGHT -> P3.4
 *
 * 外侧：90度转弯触发
 *   OUTER_LEFT  -> P3.6
 *   OUTER_RIGHT -> P3.7
 *
 * 黑线 = 1
 * 白底 = 0
 */

sbit TRACK_LEFT = P3^5;
sbit TRACK_RIGHT = P3^4;
sbit OUTER_LEFT = P3^6;
sbit OUTER_RIGHT = P3^7;

void tracking_init(void)
{
    TRACK_LEFT = 1;
    TRACK_RIGHT = 1;
    OUTER_LEFT = 1;
    OUTER_RIGHT = 1;
}

unsigned char tracking_read_pattern(void)
{
    unsigned char left;
    unsigned char right;

    left = TRACK_LEFT ? 1 : 0;
    right = TRACK_RIGHT ? 1 : 0;

    return (unsigned char)((left << 1) | right);
}

unsigned char tracking_outer_left(void)
{
    return OUTER_LEFT ? 1 : 0;
}

unsigned char tracking_outer_right(void)
{
    return OUTER_RIGHT ? 1 : 0;
}
