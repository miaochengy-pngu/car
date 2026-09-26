#include <REG52.H>
#include "timer.h"

/*
 * 11.0592 MHz、传统 12T：
 * timer clock = 11.0592 MHz / 12 = 921.6 kHz
 * 约 1 ms 需要 922 个计数，reload = 65536 - 922 = 0xFC66。
 *
 * Timer1 用于状态机计时；Timer0 保留给 PWM。
 */
#define TIMER1_RELOAD_H  0xFC
#define TIMER1_RELOAD_L  0x66

static volatile unsigned int g_system_ms = 0;

void timer1_init(void)
{
    g_system_ms = 0;

    /* 只修改 TMOD 高四位，保留 Timer0 配置。 */
    TMOD &= 0x0F;
    TMOD |= 0x10;       /* Timer1 mode 1, 16-bit timer */

    TH1 = TIMER1_RELOAD_H;
    TL1 = TIMER1_RELOAD_L;

    TF1 = 0;
    ET1 = 1;
    EA = 1;
    TR1 = 1;
}

unsigned int timer1_get_ms(void)
{
    unsigned int now;
    unsigned char old_et1;

    /*
     * 8051 是 8 位 CPU，16 位变量读取不是原子的。
     * 暂时屏蔽 Timer1 中断，避免读到撕裂值。
     */
    old_et1 = ET1;
    ET1 = 0;
    now = g_system_ms;
    ET1 = old_et1;

    return now;
}

void Timer1_ISR(void) interrupt 3 using 2
{
    TH1 = TIMER1_RELOAD_H;
    TL1 = TIMER1_RELOAD_L;
    g_system_ms++;
}
