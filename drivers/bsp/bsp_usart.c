#include "bsp_usart.h"
#include "bsp_timer.h"
#include <stdarg.h>
#include <stdio.h>

static void bsp_usart_clock_enable(uint32_t usart)
{
    if (usart == USART0) {
        rcu_periph_clock_enable(RCU_USART0);
    } else if (usart == USART1) {
        rcu_periph_clock_enable(RCU_USART1);
    } else if (usart == UART3) {
        rcu_periph_clock_enable(RCU_UART3);
    } else if (usart == UART4) {
        rcu_periph_clock_enable(RCU_UART4);
    }
}

static void bsp_usart_gpio_clock_enable(uint32_t port)
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

static void bsp_usart_gpio_config(uint32_t port, uint32_t pin, uint32_t af)
{
    bsp_usart_gpio_clock_enable(port);
    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
    gpio_af_set(port, af, pin);
}

void bsp_usart_init(const bsp_usart_cfg_t *cfg)
{
    bsp_usart_clock_enable(cfg->usart);
    bsp_usart_gpio_config(cfg->tx_port, cfg->tx_pin, cfg->tx_af);
    bsp_usart_gpio_config(cfg->rx_port, cfg->rx_pin, cfg->rx_af);

    usart_deinit(cfg->usart);
    usart_baudrate_set(cfg->usart, cfg->baud);
    usart_word_length_set(cfg->usart, USART_WL_8BIT);
    usart_stop_bit_set(cfg->usart, USART_STB_1BIT);
    usart_parity_config(cfg->usart, USART_PM_NONE);
    usart_oversample_config(cfg->usart, USART_OVSMOD_16);
    usart_transmit_config(cfg->usart, USART_TRANSMIT_ENABLE);
    usart_receive_config(cfg->usart, USART_RECEIVE_ENABLE);
    usart_enable(cfg->usart);
}

void bsp_usart_putc(uint32_t usart, uint8_t c)
{
    while (usart_flag_get(usart, USART_FLAG_TBE) == RESET) {
    }
    usart_data_transmit(usart, (uint16_t)c);
}

uint8_t bsp_usart_getc(uint32_t usart)
{
    while (usart_flag_get(usart, USART_FLAG_RBNE) == RESET) {
    }
    return (uint8_t)usart_data_receive(usart);
}

void bsp_usart_send(uint32_t usart, const uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        bsp_usart_putc(usart, buf[i]);
    }
}

uint16_t bsp_usart_receive(uint32_t usart, uint8_t *buf, uint16_t len, uint32_t timeout_ms)
{
    uint16_t received = 0U;

    while (received < len) {
        uint32_t wait = timeout_ms * 100U;
        while ((usart_flag_get(usart, USART_FLAG_RBNE) == RESET) && (wait > 0U)) {
            bsp_timer_delay_us(TIMER6, 10U);
            wait--;
        }

        if (usart_flag_get(usart, USART_FLAG_RBNE) == RESET) {
            break;
        }

        buf[received++] = (uint8_t)usart_data_receive(usart);
    }

    return received;
}

int bsp_usart_printf(uint32_t usart, const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (n > 0) {
        uint16_t len = (n < (int)sizeof(buf)) ? (uint16_t)n : (uint16_t)(sizeof(buf) - 1U);
        bsp_usart_send(usart, (const uint8_t *)buf, len);
    }
    return n;
}
