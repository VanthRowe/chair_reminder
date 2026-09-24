#ifndef BSP_LPTIMER_H
#define BSP_LPTIMER_H

#include <stdint.h>
#include "gd32l23x.h"

/* LPTIMER PWM 输出（LPTIMER_OUT 引脚），内核时钟用复位默认 LPTIMERSEL=00（CK_APB2） */
void bsp_lptimer_pwm_init(uint32_t gpio_port, uint32_t gpio_pin, uint32_t af_num,
                          uint32_t freq_hz, uint8_t duty);

/* 占空比 0~100，运行中可直接调 */
void bsp_lptimer_set_duty(uint8_t duty);

#endif /* BSP_LPTIMER_H */
