#include <REG52.H>
#include "tracking.h"

/*
 * 你的 A11 板：
 *   OTL -> P3.5
 *   OTR -> P3.4
 *
 * 实车已确认：
 *   灯亮 = 0
 *   灯灭 = 1
 *
 * 因此直接返回两位原始数字量：
 *   00 两灯亮
 *   10 左灯灭
 *   01 右灯灭
 *   11 两灯灭
 */
sbit TRACK_LEFT  = P3^5;
sbit TRACK_RIGHT = P3^4;

void tracking_init(void)
{
    TRACK_LEFT = 1;
    TRACK_RIGHT = 1;
}

unsigned char tracking_read_pattern(void)
{
    unsigned char left;
    unsigned char right;

    left = TRACK_LEFT ? 1 : 0;
    right = TRACK_RIGHT ? 1 : 0;

    return (unsigned char)((left << 1) | right);
}
