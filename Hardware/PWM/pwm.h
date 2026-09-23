#ifndef __PWM_H__
#define __PWM_H__

/*
 * 软件 PWM 由 Timer0 中断持续输出。
 * 主循环只调用这些函数修改目标占空比。
 */
void pwm_init(void);
void pwm_set_left(unsigned char duty);
void pwm_set_right(unsigned char duty);
void pwm_set_both(unsigned char left_duty, unsigned char right_duty);

/*
 * Timer0 约每 1 ms 自增一次。
 * 返回 8 位毫秒计数（0~255 循环），用于短时间非阻塞状态机。
 */
unsigned char pwm_millis8(void);

#endif
