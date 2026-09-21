#include <REG52.H>
#include "config.h"
#include "motor.h"

/*
 * A11 main-board motor-control header:
 *   ENB -> P1.0
 *   IN4 -> P1.1
 *   IN3 -> P1.2
 *   IN2 -> P1.3
 *   IN1 -> P1.4
 *   ENA -> P1.5
 */
sbit MOTOR_R_EN  = P1^0;   /* ENB */
sbit MOTOR_R_IN4 = P1^1;   /* IN4 */
sbit MOTOR_R_IN3 = P1^2;   /* IN3 */
sbit MOTOR_L_IN2 = P1^3;   /* IN2 */
sbit MOTOR_L_IN1 = P1^4;   /* IN1 */
sbit MOTOR_L_EN  = P1^5;   /* ENA */

void motor_enable(void)
{
    MOTOR_L_EN = 1;
    MOTOR_R_EN = 1;
}

void motor_disable(void)
{
    MOTOR_L_EN = 0;
    MOTOR_R_EN = 0;
}

/*
 * Polarity follows the A11 manual truth table.
 * Left : IN1=1, IN2=0 -> forward.
 * Right: IN3=0, IN4=1 -> forward.
 */
void left_motor_forward(void)
{
    MOTOR_L_IN1 = 1;
    MOTOR_L_IN2 = 0;
}

void left_motor_reverse(void)
{
    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 1;
}

void left_motor_stop(void)
{
    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 0;
}

void right_motor_forward(void)
{
    MOTOR_R_IN3 = 0;
    MOTOR_R_IN4 = 1;
}

void right_motor_reverse(void)
{
    MOTOR_R_IN3 = 1;
    MOTOR_R_IN4 = 0;
}

void right_motor_stop(void)
{
    MOTOR_R_IN3 = 0;
    MOTOR_R_IN4 = 0;
}

void car_stop(void)
{
    left_motor_stop();
    right_motor_stop();
}

void car_forward(void)
{
    left_motor_forward();
    right_motor_forward();
}

void car_backward(void)
{
    left_motor_reverse();
    right_motor_reverse();
}

void car_turn_left(void)
{
#if TURN_STYLE == TURN_STYLE_SPIN
    left_motor_reverse();
    right_motor_forward();
#else
    left_motor_stop();
    right_motor_forward();
#endif
}

void car_turn_right(void)
{
#if TURN_STYLE == TURN_STYLE_SPIN
    left_motor_forward();
    right_motor_reverse();
#else
    left_motor_forward();
    right_motor_stop();
#endif
}
