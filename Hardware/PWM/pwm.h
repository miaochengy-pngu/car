#ifndef __PWM_H__
#define __PWM_H__

void pwm_init(void);
void pwm_set_left(unsigned char duty);
void pwm_set_right(unsigned char duty);
void pwm_set_both(unsigned char left_duty, unsigned char right_duty);

#endif
