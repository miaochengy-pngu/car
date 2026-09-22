#include <REG52.H>
#include "pwm.h"

/* A11 主控板：ENA=P1.5，ENB=P1.0 */
sbit PWM_LEFT_EN  = P1^5;
sbit PWM_RIGHT_EN = P1^0;

/*
 * Fosc = 11.0592 MHz，传统 12T 8051：
 * Timer0 时钟约 921.6 kHz。
 *
 * 200 us 中断一次：
 *   25 个相位 -> 5 ms PWM 周期 -> 200 Hz
 *   PWM 分辨率 4%
 *
 * 每 25 次中断产生一次控制节拍：
 *   控制周期 5 ms -> 200 Hz
 */
#define TIMER0_RELOAD_H   0xFF
#define TIMER0_RELOAD_L   0x48

#define PWM_PHASES        25
#define CONTROL_DIVIDER   25

static volatile unsigned char g_left_steps = 0;
static volatile unsigned char g_right_steps = 0;
static volatile unsigned char g_pwm_phase = 0;
static volatile unsigned char g_control_divider = 0;
static volatile unsigned char g_control_tick = 0;

static unsigned char duty_to_steps(unsigned char duty)
{
    unsigned char steps;

    if (duty >= 100)
    {
        return PWM_PHASES;
    }

    steps = (unsigned char)((duty + 3) / 4);

    if (steps > PWM_PHASES)
    {
        steps = PWM_PHASES;
    }

    return steps;
}

void pwm_init(void)
{
    PWM_LEFT_EN = 0;
    PWM_RIGHT_EN = 0;

    g_left_steps = 0;
    g_right_steps = 0;
    g_pwm_phase = 0;
    g_control_divider = 0;
    g_control_tick = 0;

    TMOD &= 0xF0;
    TMOD |= 0x01;

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

    g_left_steps = duty_to_steps(duty);
}

void pwm_set_right(unsigned char duty)
{
    if (duty > 100)
    {
        duty = 100;
    }

    g_right_steps = duty_to_steps(duty);
}

void pwm_set_both(unsigned char left_duty, unsigned char right_duty)
{
    pwm_set_left(left_duty);
    pwm_set_right(right_duty);
}

unsigned char pwm_control_tick_ready(void)
{
    return g_control_tick;
}

void pwm_clear_control_tick(void)
{
    g_control_tick = 0;
}

void Timer0_ISR(void) interrupt 1 using 1
{
    TH0 = TIMER0_RELOAD_H;
    TL0 = TIMER0_RELOAD_L;

    g_pwm_phase++;

    if (g_pwm_phase >= PWM_PHASES)
    {
        g_pwm_phase = 0;
    }

    PWM_LEFT_EN = (g_pwm_phase < g_left_steps) ? 1 : 0;
    PWM_RIGHT_EN = (g_pwm_phase < g_right_steps) ? 1 : 0;

    g_control_divider++;

    if (g_control_divider >= CONTROL_DIVIDER)
    {
        g_control_divider = 0;
        g_control_tick = 1;
    }
}
