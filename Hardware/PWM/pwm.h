#ifndef __PWM_H__
#define __PWM_H__

/*
 * Software PWM on L298N ENA/ENB.
 * Duty arguments use 0..100 percent.
 */

void pwm_init(void);
void pwm_set_left(unsigned char duty);
void pwm_set_right(unsigned char duty);
void pwm_set_both(unsigned char left_duty, unsigned char right_duty);

unsigned char pwm_control_tick_ready(void);
void pwm_clear_control_tick(void);

#endif
