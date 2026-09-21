#include "delay.h"

/* Approx. 1 ms at 11.0592 MHz on a classic 12T 8051. */
void delay_1ms(void)
{
    unsigned char i;
    unsigned char j;

    i = 2;
    j = 199;

    do
    {
        while (--j);
    } while (--i);
}

void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        delay_1ms();
    }
}
