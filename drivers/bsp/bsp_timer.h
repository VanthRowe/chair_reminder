#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include <stdint.h>
#include "gd32l23x.h"

/* 初始化定时器时间基，freq_hz 为计数频率（常用 1000 表示 1ms 一个 tick） */
void bsp_timer_init(uint32_t timer_periph, uint32_t freq_hz);

void bsp_timer_start(uint32_t timer_periph);
void bsp_timer_stop(uint32_t timer_periph);
uint32_t bsp_timer_counter_get(uint32_t timer_periph);

/* 轮询延时，会重新配置定时器 tick 频率，最大 65535 个 tick */
void bsp_timer_delay_us(uint32_t timer_periph, uint32_t us);
void bsp_timer_delay_ms(uint32_t timer_periph, uint32_t ms);
void bsp_timer_delay_ticks(uint32_t timer_periph, uint32_t ticks);

#endif /* BSP_TIMER_H */
