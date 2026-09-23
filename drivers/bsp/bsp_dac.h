#ifndef BSP_DAC_H
#define BSP_DAC_H

#include <stdint.h>
#include "gd32l23x.h"

/* 初始化 DAC0_OUT0，配置对应 GPIO 为模拟模式 */
void bsp_dac_init(uint32_t gpio_port, uint32_t gpio_pin);

/* 直接设置 DAC 值 0 ~ 4095 */
void bsp_dac_set_value(uint16_t value);

/* 按 3.3V 参考电压设置输出毫伏值 */
void bsp_dac_set_voltage(uint16_t mv);

#endif /* BSP_DAC_H */
