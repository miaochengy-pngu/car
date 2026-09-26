#include "delay.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "timer.h"

void main(void)
{
    tracking_init();
    motor_init();

    /*
     * Timer1 只负责状态机毫秒计时。
     * Timer0 留给 PWM，二者不再冲突。
     */
    timer1_init();

    line_control_init();

    delay_ms(300);

    while (1)
    {
        line_control_step();
    }
}
