#include "delay.h"
#include "pwm.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

void main(void)
{
    tracking_init();
    motor_init();
    pwm_init();
    line_control_init();

    delay_ms(300);

    /*
     * 参考 STC89C52RC 开源循迹例程：
     * while(1) 只轮询传感器、修改左右目标占空比。
     * PWM 波形本身由 Timer0 中断独立产生。
     */
    while (1)
    {
        line_control_step();
    }
}
