#include "delay.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "timer.h"
#include "pwm.h"

void main(void)
{
    tracking_init();

    /*
     * Timer0：软件 PWM，真正实现 40% / 80% / 100% 电机速度。
     */
    pwm_init();
    motor_init();

    /*
     * Timer1：只负责状态机毫秒计时。
     * 与 Timer0 PWM 相互独立。
     */
    timer1_init();

    line_control_init();

    delay_ms(300);

    while (1)
    {
        line_control_step();
    }
}
