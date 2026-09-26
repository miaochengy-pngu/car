#include <REG52.H>
#include "motor.h"
#include "pwm.h"

/*
 * A11 主控板电机接口：
 *   ENB -> P1.0 -> 右轮 PWM
 *   IN4 -> P1.1
 *   IN3 -> P1.2
 *   IN2 -> P1.3
 *   IN1 -> P1.4
 *   ENA -> P1.5 -> 左轮 PWM
 *
 * 方向由 IN1~IN4 决定；
 * 速度由 pwm.c 的 Timer0 软件 PWM 决定。
 * command 范围 -100~+100：
 *   正负号 = 方向
 *   绝对值 = PWM 占空比百分比
 */

sbit MOTOR_R_IN4 = P1^1;
sbit MOTOR_R_IN3 = P1^2;
sbit MOTOR_L_IN2 = P1^3;
sbit MOTOR_L_IN1 = P1^4;

static unsigned char command_to_duty(int command)
{
    if (command < 0)
    {
        command = -command;
    }

    if (command > 100)
    {
        command = 100;
    }

    return (unsigned char)command;
}

static void set_left(int command)
{
    if (command > 0)
    {
        MOTOR_L_IN1 = 1;
        MOTOR_L_IN2 = 0;
        pwm_set_left(command_to_duty(command));
    }
    else if (command < 0)
    {
        MOTOR_L_IN1 = 0;
        MOTOR_L_IN2 = 1;
        pwm_set_left(command_to_duty(command));
    }
    else
    {
        pwm_set_left(0);
        MOTOR_L_IN1 = 0;
        MOTOR_L_IN2 = 0;
    }
}

static void set_right(int command)
{
    if (command > 0)
    {
        MOTOR_R_IN3 = 0;
        MOTOR_R_IN4 = 1;
        pwm_set_right(command_to_duty(command));
    }
    else if (command < 0)
    {
        MOTOR_R_IN3 = 1;
        MOTOR_R_IN4 = 0;
        pwm_set_right(command_to_duty(command));
    }
    else
    {
        pwm_set_right(0);
        MOTOR_R_IN3 = 0;
        MOTOR_R_IN4 = 0;
    }
}

void motor_init(void)
{
    pwm_set_both(0, 0);

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
