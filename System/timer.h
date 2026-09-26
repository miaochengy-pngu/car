#ifndef __TIMER_H__
#define __TIMER_H__

/*
 * Timer1 专门提供状态机毫秒时基。
 * Timer0 留给 PWM，不与计时器抢占。
 */
void timer1_init(void);
unsigned int timer1_get_ms(void);

#endif
