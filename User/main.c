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

    /*
     * Keep the car centered on the track during power-up.
     * This allows the firmware to learn whether the nominal
     * center sensor pattern is 00 or 11.
     */
    delay_ms(500);
    tracking_calibrate_center();

    line_control_init();
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
