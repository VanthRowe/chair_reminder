#include "bsp_gpio.h"

static void bsp_gpio_clock_enable(uint32_t port)
{
    switch (port) {
    case GPIOA:
        rcu_periph_clock_enable(RCU_GPIOA);
        break;
    case GPIOB:
        rcu_periph_clock_enable(RCU_GPIOB);
        break;
    case GPIOC:
        rcu_periph_clock_enable(RCU_GPIOC);
        break;
    case GPIOD:
        rcu_periph_clock_enable(RCU_GPIOD);
        break;
    case GPIOF:
        rcu_periph_clock_enable(RCU_GPIOF);
        break;
    default:
        break;
    }
}

static uint32_t bsp_gpio_pull_map(bsp_gpio_pull_t pull)
{
    switch (pull) {
    case BSP_GPIO_PULL_UP:
        return GPIO_PUPD_PULLUP;
    case BSP_GPIO_PULL_DOWN:
        return GPIO_PUPD_PULLDOWN;
    default:
        return GPIO_PUPD_NONE;
    }
}

void bsp_gpio_init_output(const bsp_gpio_t *gpio)
{
    bsp_gpio_clock_enable(gpio->port);
    gpio_mode_set(gpio->port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, gpio->pin);
    gpio_output_options_set(gpio->port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, gpio->pin);
    bsp_gpio_off(gpio);
}

void bsp_gpio_init_input(const bsp_gpio_t *gpio, bsp_gpio_pull_t pull)
{
    bsp_gpio_clock_enable(gpio->port);
    gpio_mode_set(gpio->port, GPIO_MODE_INPUT, bsp_gpio_pull_map(pull), gpio->pin);
}

void bsp_gpio_set(const bsp_gpio_t *gpio)
{
    gpio_bit_set(gpio->port, gpio->pin);
}

void bsp_gpio_reset(const bsp_gpio_t *gpio)
{
    gpio_bit_reset(gpio->port, gpio->pin);
}

void bsp_gpio_write(const bsp_gpio_t *gpio, uint8_t level)
{
    gpio_bit_write(gpio->port, gpio->pin, (level != 0U) ? SET : RESET);
}

void bsp_gpio_toggle(const bsp_gpio_t *gpio)
{
    gpio_bit_toggle(gpio->port, gpio->pin);
}

uint8_t bsp_gpio_read(const bsp_gpio_t *gpio)
{
    return (gpio_input_bit_get(gpio->port, gpio->pin) != RESET) ? 1U : 0U;
}

void bsp_gpio_on(const bsp_gpio_t *gpio)
{
    if (gpio->active_low) {
        bsp_gpio_reset(gpio);
    } else {
        bsp_gpio_set(gpio);
    }
}

void bsp_gpio_off(const bsp_gpio_t *gpio)
{
    if (gpio->active_low) {
        bsp_gpio_set(gpio);
    } else {
        bsp_gpio_reset(gpio);
    }
}
