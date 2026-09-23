#ifndef BSP_USART_H
#define BSP_USART_H

#include <stdint.h>
#include "gd32l23x.h"

typedef struct {
    uint32_t usart;
    uint32_t baud;

    uint32_t tx_port;
    uint32_t tx_pin;
    uint32_t tx_af;

    uint32_t rx_port;
    uint32_t rx_pin;
    uint32_t rx_af;
} bsp_usart_cfg_t;

void bsp_usart_init(const bsp_usart_cfg_t *cfg);

void bsp_usart_putc(uint32_t usart, uint8_t c);
uint8_t bsp_usart_getc(uint32_t usart);

void bsp_usart_send(uint32_t usart, const uint8_t *buf, uint16_t len);
uint16_t bsp_usart_receive(uint32_t usart, uint8_t *buf, uint16_t len, uint32_t timeout_ms);

int bsp_usart_printf(uint32_t usart, const char *fmt, ...);

#endif /* BSP_USART_H */
