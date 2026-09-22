#include "pid.h"

#define PID_GAIN_SCALE 100L

static long limit_long(long value, long limit)
{
    if (value > limit)
    {
        return limit;
    }

    if (value < -limit)
    {
        return -limit;
    }

    return value;
}

static int limit_int(int value, int limit)
{
    if (value > limit)
    {
        return limit;
    }

    if (value < -limit)
    {
        return -limit;
    }

    return value;
}

void pid_init(PID_Controller *pid,
              int kp,
              int ki,
              int kd,
              int integral_limit,
              int output_limit)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->previous_error = 0;
    pid->integral = 0;

    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
}

void pid_reset(PID_Controller *pid)
{
    pid->previous_error = 0;
    pid->integral = 0;
}

int pid_update(PID_Controller *pid, int error)
{
    int derivative;
    long output;

    pid->integral += error;
    pid->integral = limit_long(pid->integral, pid->integral_limit);

    derivative = error - pid->previous_error;
    pid->previous_error = error;

    output = (long)pid->kp * error
           + (long)pid->ki * pid->integral
           + (long)pid->kd * derivative;

    output /= PID_GAIN_SCALE;

    return limit_int((int)output, pid->output_limit);
}
