#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include <stdint.h>
#include "gd32l23x.h"

typedef enum {
    BSP_GPIO_PULL_NONE = 0,
    BSP_GPIO_PULL_UP,
    BSP_GPIO_PULL_DOWN
} bsp_gpio_pull_t;

typedef struct {
    uint32_t port;
    uint32_t pin;
    uint8_t  active_low; /* 1: 低电平为“开”，用于 LED */
} bsp_gpio_t;

void bsp_gpio_init_output(const bsp_gpio_t *gpio);
void bsp_gpio_init_input(const bsp_gpio_t *gpio, bsp_gpio_pull_t pull);

void bsp_gpio_set(const bsp_gpio_t *gpio);
void bsp_gpio_reset(const bsp_gpio_t *gpio);
void bsp_gpio_write(const bsp_gpio_t *gpio, uint8_t level);
void bsp_gpio_toggle(const bsp_gpio_t *gpio);
uint8_t bsp_gpio_read(const bsp_gpio_t *gpio);

/* 根据 active_low 语义控制“开/关”，适合 LED */
void bsp_gpio_on(const bsp_gpio_t *gpio);
void bsp_gpio_off(const bsp_gpio_t *gpio);

#endif /* BSP_GPIO_H */
