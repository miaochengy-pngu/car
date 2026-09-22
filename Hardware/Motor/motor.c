#include <REG52.H>
#include "pwm.h"
#include "motor.h"

/*
 * A11 main-board motor-control header:
 *   ENB -> P1.0   (software PWM, Hardware/PWM)
 *   IN4 -> P1.1
 *   IN3 -> P1.2
 *   IN2 -> P1.3
 *   IN1 -> P1.4
 *   ENA -> P1.5   (software PWM, Hardware/PWM)
 */
sbit MOTOR_R_IN4 = P1^1;
sbit MOTOR_R_IN3 = P1^2;
sbit MOTOR_L_IN2 = P1^3;
sbit MOTOR_L_IN1 = P1^4;

static int clamp_command(int command)
{
    if (command > 100)
    {
        return 100;
    }

    if (command < -100)
    {
        return -100;
    }

    return command;
}

static unsigned char abs_command(int command)
{
    if (command < 0)
    {
        command = -command;
    }

    return (unsigned char)command;
}

static void set_left_direction(int command)
{
    if (command > 0)
    {
        /* A11 manual: IN1=1, IN2=0 -> left forward */
        MOTOR_L_IN1 = 1;
        MOTOR_L_IN2 = 0;
    }
    else if (command < 0)
    {
        MOTOR_L_IN1 = 0;
        MOTOR_L_IN2 = 1;
    }
    else
    {
        MOTOR_L_IN1 = 0;
        MOTOR_L_IN2 = 0;
    }
}

static void set_right_direction(int command)
{
    if (command > 0)
    {
        /* A11 manual: IN3=0, IN4=1 -> right forward */
        MOTOR_R_IN3 = 0;
        MOTOR_R_IN4 = 1;
    }
    else if (command < 0)
    {
        MOTOR_R_IN3 = 1;
        MOTOR_R_IN4 = 0;
    }
    else
    {
        MOTOR_R_IN3 = 0;
        MOTOR_R_IN4 = 0;
    }
}

void motor_init(void)
{
    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 0;
    MOTOR_R_IN3 = 0;
    MOTOR_R_IN4 = 0;

    pwm_set_both(0, 0);
}

void motor_set(int left_command, int right_command)
{
    left_command = clamp_command(left_command);
    right_command = clamp_command(right_command);

    set_left_direction(left_command);
    set_right_direction(right_command);

    pwm_set_left(abs_command(left_command));
    pwm_set_right(abs_command(right_command));
}

void motor_stop(void)
{
    motor_set(0, 0);
}
