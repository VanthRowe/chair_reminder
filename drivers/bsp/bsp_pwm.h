#ifndef BSP_PWM_H
#define BSP_PWM_H

#include <stdint.h>
#include "gd32l23x.h"

typedef struct {
    uint32_t timer;        /* TIMERx */
    uint16_t channel;      /* TIMER_CH_0 ~ TIMER_CH_3 */
    uint32_t gpio_port;    /* GPIOx */
    uint32_t gpio_pin;     /* GPIO_PIN_x */
    uint32_t af_num;       /* GPIO_AF_x */
    uint32_t oc_polarity;  /* TIMER_OC_POLARITY_HIGH / TIMER_OC_POLARITY_LOW */
    uint32_t freq_hz;      /* PWM 频率 */
    uint8_t  duty;         /* 占空比 0 ~ 100 */
} bsp_pwm_cfg_t;

void bsp_pwm_init(const bsp_pwm_cfg_t *cfg);
void bsp_pwm_channel_add(uint32_t timer, uint16_t channel,
                         uint32_t gpio_port, uint32_t gpio_pin, uint32_t af_num,
                         uint32_t oc_polarity);
void bsp_pwm_start(uint32_t timer);
void bsp_pwm_stop(uint32_t timer);
void bsp_pwm_set_duty(uint32_t timer, uint16_t channel, uint8_t duty);
void bsp_pwm_set_freq(uint32_t timer, uint16_t channel, uint32_t oc_polarity,
                      uint32_t freq_hz, uint8_t duty);

#endif /* BSP_PWM_H */
