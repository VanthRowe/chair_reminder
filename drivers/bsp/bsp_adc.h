#ifndef BSP_ADC_H
#define BSP_ADC_H

#include <stdint.h>
#include "gd32l23x.h"

/* 初始化指定 ADC 通道对应的 GPIO 为模拟输入，并初始化 ADC */
void bsp_adc_init(uint8_t channel, uint32_t gpio_port, uint32_t gpio_pin);

/* 设置通道采样周期，sample_time 使用 ADC_SAMPLETIME_xxx */
void bsp_adc_sample_time_config(uint8_t channel, uint32_t sample_time);

/* 软件触发一次并返回 12bit 转换结果 */
uint16_t bsp_adc_read(uint8_t channel);

#endif /* BSP_ADC_H */
