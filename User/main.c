#include "delay.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "timer.h"

void main(void)
{
    tracking_init();
    motor_init();
    timer0_init();
    line_control_init();

    delay_ms(300);

    while (1)
    {
        line_control_step();
    }
}
