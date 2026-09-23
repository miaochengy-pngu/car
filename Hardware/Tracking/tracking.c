#include <REG52.H>
#include "config.h"
#include "tracking.h"

/*
 * VER:2.3 主控板红外循迹接口：
 *   OTL -> P3.5
 *   OTR -> P3.4
 */
sbit TRACK_LEFT  = P3^5;
sbit TRACK_RIGHT = P3^4;

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
    /* 8051 准双向口作输入时先写 1 */
    TRACK_LEFT = 1;
    TRACK_RIGHT = 1;
}

unsigned char tracking_read_pattern(void)
{
    unsigned char left_black;
    unsigned char right_black;

    left_black = is_black(TRACK_LEFT);
    right_black = is_black(TRACK_RIGHT);

    return (unsigned char)((left_black << 1) | right_black);
}
