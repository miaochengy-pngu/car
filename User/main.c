#include "config.h"
#include "delay.h"
#include "motor.h"
#include "tracking.h"

void main(void)
{
    tracking_init();

    motor_enable();
    car_stop();

    /* Wait for the power rail and LM393 comparators to stabilize. */
    delay_ms(800);

    while (1)
    {
        tracking_control();
        delay_ms(CONTROL_PERIOD_MS);
    }
}
