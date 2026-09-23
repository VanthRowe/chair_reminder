#ifndef BSP_SPI_H
#define BSP_SPI_H

#include <stdint.h>
#include "gd32l23x.h"

typedef enum {
    BSP_SPI_MODE_0 = 0, /* CPOL=0, CPHA=0 */
    BSP_SPI_MODE_1,     /* CPOL=0, CPHA=1 */
    BSP_SPI_MODE_2,     /* CPOL=1, CPHA=0 */
    BSP_SPI_MODE_3      /* CPOL=1, CPHA=1 */
} bsp_spi_mode_t;

typedef struct {
    uint32_t spi;

    uint32_t sck_port;
    uint32_t sck_pin;
    uint32_t sck_af;

    uint32_t miso_port;
    uint32_t miso_pin;
    uint32_t miso_af;

    uint32_t mosi_port;
    uint32_t mosi_pin;
    uint32_t mosi_af;

    uint32_t nss_port;
    uint32_t nss_pin;

    bsp_spi_mode_t mode;
    uint32_t prescale;  /* SPI_PSC_2/4/8/16/32/64/128/256 */
} bsp_spi_cfg_t;

void bsp_spi_init(const bsp_spi_cfg_t *cfg);

void bsp_spi_nss_set(uint32_t spi, uint8_t level);
uint8_t bsp_spi_transfer(uint32_t spi, uint8_t tx);
void bsp_spi_send(uint32_t spi, const uint8_t *buf, uint16_t len);
void bsp_spi_receive(uint32_t spi, uint8_t *buf, uint16_t len);
void bsp_spi_transfer_buf(uint32_t spi, const uint8_t *tx, uint8_t *rx, uint16_t len);

#endif /* BSP_SPI_H */
