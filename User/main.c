#include "delay.h"
#include "pwm.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

void main(void)
{
    tracking_init();
    pwm_init();
    motor_init();
    line_control_init();

    /* 上电稳定 */
    delay_ms(300);

    pwm_clear_control_tick();

    while (1)
    {
        if (pwm_control_tick_ready())
        {
            pwm_clear_control_tick();
            line_control_step();
        }
    }
}
