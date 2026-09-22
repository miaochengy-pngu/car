#ifndef __PID_H__
#define __PID_H__

typedef struct
{
    int kp;                 /* gain x100 */
    int ki;                 /* gain x100 */
    int kd;                 /* gain x100 */

    int previous_error;
    long integral;

    int integral_limit;
    int output_limit;
} PID_Controller;

void pid_init(PID_Controller *pid,
              int kp,
              int ki,
              int kd,
              int integral_limit,
              int output_limit);

void pid_reset(PID_Controller *pid);
int pid_update(PID_Controller *pid, int error);

#endif
