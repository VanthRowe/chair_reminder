#include "bsp_spi.h"
#include <stddef.h>

static void bsp_spi_gpio_clock_enable(uint32_t port)
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

static void bsp_spi_clock_enable(uint32_t spi)
{
    if (spi == SPI0) {
        rcu_periph_clock_enable(RCU_SPI0);
    } else if (spi == SPI1) {
        rcu_periph_clock_enable(RCU_SPI1);
    }
}

static void bsp_spi_gpio_config(uint32_t port, uint32_t pin, uint32_t af)
{
    bsp_spi_gpio_clock_enable(port);
    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
    gpio_af_set(port, af, pin);
}

static uint32_t bsp_spi_mode_map(bsp_spi_mode_t mode)
{
    switch (mode) {
    case BSP_SPI_MODE_1:
        return SPI_CK_PL_LOW_PH_2EDGE;
    case BSP_SPI_MODE_2:
        return SPI_CK_PL_HIGH_PH_1EDGE;
    case BSP_SPI_MODE_3:
        return SPI_CK_PL_HIGH_PH_2EDGE;
    default:
        return SPI_CK_PL_LOW_PH_1EDGE;
    }
}

static uint32_t g_spi_nss_port[2] = {0U, 0U};
static uint32_t g_spi_nss_pin[2]  = {0U, 0U};

void bsp_spi_init(const bsp_spi_cfg_t *cfg)
{
    uint32_t index = (cfg->spi == SPI0) ? 0U : 1U;

    bsp_spi_clock_enable(cfg->spi);
    bsp_spi_gpio_config(cfg->sck_port, cfg->sck_pin, cfg->sck_af);
    bsp_spi_gpio_config(cfg->miso_port, cfg->miso_pin, cfg->miso_af);
    bsp_spi_gpio_config(cfg->mosi_port, cfg->mosi_pin, cfg->mosi_af);

    g_spi_nss_port[index] = cfg->nss_port;
    g_spi_nss_pin[index]  = cfg->nss_pin;

    if (cfg->nss_port != 0U) {
        bsp_spi_gpio_clock_enable(cfg->nss_port);
        gpio_mode_set(cfg->nss_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, cfg->nss_pin);
        gpio_output_options_set(cfg->nss_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, cfg->nss_pin);
        gpio_bit_set(cfg->nss_port, cfg->nss_pin);
    }

    spi_i2s_deinit(cfg->spi);

    spi_parameter_struct spi_init_struct;
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init_struct.clock_polarity_phase = bsp_spi_mode_map(cfg->mode);
    spi_init_struct.prescale             = cfg->prescale;
    spi_init(cfg->spi, &spi_init_struct);

    spi_enable(cfg->spi);
}

void bsp_spi_nss_set(uint32_t spi, uint8_t level)
{
    uint32_t index = (spi == SPI0) ? 0U : 1U;
    uint32_t port  = g_spi_nss_port[index];
    uint32_t pin   = g_spi_nss_pin[index];

    if (port != 0U) {
        if (level != 0U) {
            gpio_bit_set(port, pin);
        } else {
            gpio_bit_reset(port, pin);
        }
    }
}

uint8_t bsp_spi_transfer(uint32_t spi, uint8_t tx)
{
    while (spi_i2s_flag_get(spi, SPI_FLAG_TBE) == RESET) {
    }
    spi_i2s_data_transmit(spi, (uint16_t)tx);

    while (spi_i2s_flag_get(spi, SPI_FLAG_RBNE) == RESET) {
    }
    return (uint8_t)spi_i2s_data_receive(spi);
}

void bsp_spi_send(uint32_t spi, const uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0U; i < len; i++) {
        (void)bsp_spi_transfer(spi, buf[i]);
    }
}

void bsp_spi_receive(uint32_t spi, uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0U; i < len; i++) {
        buf[i] = bsp_spi_transfer(spi, 0xFFU);
    }
}

void bsp_spi_transfer_buf(uint32_t spi, const uint8_t *tx, uint8_t *rx, uint16_t len)
{
    for (uint16_t i = 0U; i < len; i++) {
        uint8_t data = bsp_spi_transfer(spi, (tx != NULL) ? tx[i] : 0xFFU);
        if (rx != NULL) {
            rx[i] = data;
        }
    }
}
