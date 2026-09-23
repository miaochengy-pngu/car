#include <REG52.H>
#include "motor.h"

/*
 * A11 主控板电机接口：
 *   ENB -> P1.0
 *   IN4 -> P1.1
 *   IN3 -> P1.2
 *   IN2 -> P1.3
 *   IN1 -> P1.4
 *   ENA -> P1.5
 *
 * 当前为了排除软件 PWM / Timer0 问题，
 * ENA / ENB 直接高电平全速使能。
 */
sbit MOTOR_R_EN  = P1^0;
sbit MOTOR_R_IN4 = P1^1;
sbit MOTOR_R_IN3 = P1^2;
sbit MOTOR_L_IN2 = P1^3;
sbit MOTOR_L_IN1 = P1^4;
sbit MOTOR_L_EN  = P1^5;

static void set_left(int command)
{
    if (command > 0)
    {
        /* IN1=1, IN2=0 -> 左轮正转 */
        MOTOR_L_IN1 = 1;
        MOTOR_L_IN2 = 0;
        MOTOR_L_EN = 1;
    }
    else if (command < 0)
    {
        MOTOR_L_IN1 = 0;
        MOTOR_L_IN2 = 1;
        MOTOR_L_EN = 1;
    }
    else
    {
        MOTOR_L_EN = 0;
        MOTOR_L_IN1 = 0;
        MOTOR_L_IN2 = 0;
    }
}

static void set_right(int command)
{
    if (command > 0)
    {
        /* IN3=0, IN4=1 -> 右轮正转 */
        MOTOR_R_IN3 = 0;
        MOTOR_R_IN4 = 1;
        MOTOR_R_EN = 1;
    }
    else if (command < 0)
    {
        MOTOR_R_IN3 = 1;
        MOTOR_R_IN4 = 0;
        MOTOR_R_EN = 1;
    }
    else
    {
        MOTOR_R_EN = 0;
        MOTOR_R_IN3 = 0;
        MOTOR_R_IN4 = 0;
    }
}

void motor_init(void)
{
    MOTOR_L_EN = 0;
    MOTOR_R_EN = 0;

    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 0;
    MOTOR_R_IN3 = 0;
    MOTOR_R_IN4 = 0;
}

void motor_set(int left_command, int right_command)
{
    set_left(left_command);
    set_right(right_command);
}

void motor_stop(void)
{
    motor_set(0, 0);
}
