#ifndef __MOTOR_H__
#define __MOTOR_H__

/*
 * Signed motor command:
 *   +100 = full forward
 *      0 = stop
 *   -100 = full reverse
 */
void motor_init(void);
void motor_set(int left_command, int right_command);
void motor_stop(void);

#endif
