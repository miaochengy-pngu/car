#include <REG52.H>
#include "tracking.h"

/*
 * VER:2.3 主控板红外循迹接口：
 *   OTL -> P3.5
 *   OTR -> P3.4
 *
 * 实车已确认：
 *   黑线 / 无反射 -> 灯灭 -> MCU 读 1
 *   白底          -> 灯亮 -> MCU 读 0
 */
sbit TRACK_LEFT  = P3^5;
sbit TRACK_RIGHT = P3^4;

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

    /*
     * 当前硬件逻辑已确认：高电平就是“黑/无反射”。
     * 直接读取，避免老版本 C51 对 #if 宏表达式产生 C322 warning。
     */
    left_black = TRACK_LEFT ? 1 : 0;
    right_black = TRACK_RIGHT ? 1 : 0;

    return (unsigned char)((left_black << 1) | right_black);
}
