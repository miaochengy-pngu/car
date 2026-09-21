#include "config.h"
#include "delay.h"
#include "motor.h"
#include "tracking.h"

void main(void)
{
    tracking_init();

    motor_enable();
    car_stop();

    /* Allow power and sensor comparators to settle. */
    delay_ms(800);

    while (1)
    {
        tracking_control();
        delay_ms(CONTROL_PERIOD_MS);
    }
}
