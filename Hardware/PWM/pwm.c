#include <REG52.H>
#include "pwm.h"

/*
 * 参考公开的 STC89C52RC 循迹小车例程写法：
 * Timer0 中断只负责软件 PWM 和 1 ms 系统节拍；
 * 主循环只负责读循迹传感器并改占空比。
 *
 * A11 主控板：
 *   ENA -> P1.5 -> 左电机 PWM
 *   ENB -> P1.0 -> 右电机 PWM
 */
sbit PWM_LEFT_EN  = P1^5;
sbit PWM_RIGHT_EN = P1^0;

/*
 * 11.0592 MHz、传统 12T：
 * 约 1 ms Timer0 中断。
 *
 * 20 级 PWM：
 *   20 x 1 ms = 20 ms
 *   PWM 频率约 50 Hz
 *   分辨率 5%
 */
#define TIMER0_RELOAD_H  0xFC
#define TIMER0_RELOAD_L  0x66
#define PWM_STEPS         20

static volatile unsigned char g_left_duty_steps = 0;
static volatile unsigned char g_right_duty_steps = 0;
static volatile unsigned char g_pwm_counter = 0;
static volatile unsigned char g_ms_tick = 0;

static unsigned char percent_to_steps(unsigned char percent)
{
    unsigned char steps;

    if (percent >= 100)
    {
        return PWM_STEPS;
    }

    steps = (unsigned char)((percent + 2) / 5);

    if (steps > PWM_STEPS)
    {
        steps = PWM_STEPS;
    }

    return steps;
}

void pwm_init(void)
{
    PWM_LEFT_EN = 0;
    PWM_RIGHT_EN = 0;

    g_left_duty_steps = 0;
    g_right_duty_steps = 0;
    g_pwm_counter = 0;
    g_ms_tick = 0;

    TMOD &= 0xF0;
    TMOD |= 0x01;       /* Timer0 mode 1 */

    TH0 = TIMER0_RELOAD_H;
    TL0 = TIMER0_RELOAD_L;

    TF0 = 0;
    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

void pwm_set_left(unsigned char duty)
{
    if (duty > 100)
    {
        duty = 100;
    }

    g_left_duty_steps = percent_to_steps(duty);
}

void pwm_set_right(unsigned char duty)
{
    if (duty > 100)
    {
        duty = 100;
    }

    g_right_duty_steps = percent_to_steps(duty);
}

void pwm_set_both(unsigned char left_duty, unsigned char right_duty)
{
    pwm_set_left(left_duty);
    pwm_set_right(right_duty);
}

unsigned char pwm_millis8(void)
{
    return g_ms_tick;
}

void Timer0_ISR(void) interrupt 1 using 1
{
    TH0 = TIMER0_RELOAD_H;
    TL0 = TIMER0_RELOAD_L;

    g_ms_tick++;

    g_pwm_counter++;

    if (g_pwm_counter >= PWM_STEPS)
    {
        g_pwm_counter = 0;
    }

    if (g_pwm_counter < g_left_duty_steps)
    {
        PWM_LEFT_EN = 1;
    }
    else
    {
        PWM_LEFT_EN = 0;
    }

    if (g_pwm_counter < g_right_duty_steps)
    {
        PWM_RIGHT_EN = 1;
    }
    else
    {
        PWM_RIGHT_EN = 0;
    }
}
