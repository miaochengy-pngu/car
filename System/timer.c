#include <reg52.h>
#include "timer.h"

volatile unsigned int system_ms = 0;

void timer0_init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x01;

    TH0 = (65536 - 1000) / 256;
    TL0 = (65536 - 1000) % 256;

    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

void timer0_isr(void) interrupt 1
{
    TH0 = (65536 - 1000) / 256;
    TL0 = (65536 - 1000) % 256;
    system_ms++;
}

unsigned int timer0_get_ms(void)
{
    return system_ms;
}
