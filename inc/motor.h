#ifndef __MOTOR_H__
#define __MOTOR_H__

void motor_enable(void);
void motor_disable(void);

void left_motor_forward(void);
void left_motor_reverse(void);
void left_motor_stop(void);

void right_motor_forward(void);
void right_motor_reverse(void);
void right_motor_stop(void);

void car_stop(void);
void car_forward(void);
void car_backward(void);
void car_turn_left(void);
void car_turn_right(void);

#endif
