#include "bsp_i2c.h"

#define BSP_I2C_TIMEOUT  0x10000U

static void bsp_i2c_gpio_clock_enable(uint32_t port)
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

static void bsp_i2c_clock_enable(uint32_t i2c)
{
    if (i2c == I2C0) {
        rcu_periph_clock_enable(RCU_I2C0);
    } else if (i2c == I2C1) {
        rcu_periph_clock_enable(RCU_I2C1);
    } else if (i2c == I2C2) {
        rcu_periph_clock_enable(RCU_I2C2);
    }
}

static void bsp_i2c_gpio_config(uint32_t port, uint32_t pin, uint32_t af)
{
    bsp_i2c_gpio_clock_enable(port);
    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin);
    gpio_output_options_set(port, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, pin);
    gpio_af_set(port, af, pin);
}

static void bsp_i2c_timing_config_by_speed(uint32_t i2c, bsp_i2c_speed_t speed)
{
    uint32_t psc, scll, sclh, scldely, sdadely;

    if (speed == BSP_I2C_SPEED_400K) {
        /* APB1 = 32 MHz, tI2CCLK = 16 MHz, SCL period = 2.5 us */
        psc      = 1U;
        scll     = 9U;
        sclh     = 8U;
        scldely  = 0U;
        sdadely  = 0U;
    } else {
        /* APB1 = 32 MHz, tI2CCLK = 8 MHz, SCL period = 10 us */
        psc      = 3U;
        scll     = 40U;
        sclh     = 37U;
        scldely  = 0U;
        sdadely  = 0U;
    }

    i2c_timing_config(i2c, psc, scldely, sdadely);
    i2c_master_clock_config(i2c, sclh, scll);
}

void bsp_i2c_init(const bsp_i2c_cfg_t *cfg)
{
    bsp_i2c_clock_enable(cfg->i2c);
    bsp_i2c_gpio_config(cfg->scl_port, cfg->scl_pin, cfg->scl_af);
    bsp_i2c_gpio_config(cfg->sda_port, cfg->sda_pin, cfg->sda_af);

    i2c_deinit(cfg->i2c);
    i2c_clock_timeout_disable(cfg->i2c);
    i2c_analog_noise_filter_disable(cfg->i2c);
    i2c_digital_noise_filter_config(cfg->i2c, FILTER_DISABLE);
    bsp_i2c_timing_config_by_speed(cfg->i2c, cfg->speed);
    i2c_enable(cfg->i2c);
}

static int bsp_i2c_wait_flag(uint32_t i2c, uint32_t flag, uint8_t state)
{
    uint32_t timeout = BSP_I2C_TIMEOUT;

    while ((i2c_flag_get(i2c, flag) == state) && (timeout > 0U)) {
        timeout--;
    }

    return (timeout > 0U) ? 0 : -1;
}

static int bsp_i2c_wait_busy(uint32_t i2c)
{
    return bsp_i2c_wait_flag(i2c, I2C_FLAG_I2CBSY, SET);
}

static int bsp_i2c_master_start(uint32_t i2c, uint8_t addr, uint32_t dir, uint16_t len)
{
    if (bsp_i2c_wait_busy(i2c) != 0) {
        return -1;
    }

    i2c_automatic_end_enable(i2c);
    i2c_transfer_byte_number_config(i2c, len);
    i2c_master_addressing(i2c, (uint32_t)(addr << 1U), dir);
    i2c_start_on_bus(i2c);

    return 0;
}

int bsp_i2c_write(uint32_t i2c, uint8_t addr, const uint8_t *buf, uint16_t len)
{
    if (len == 0U) {
        return 0;
    }
    if (len > 255U) {
        len = 255U;
    }

    if (bsp_i2c_master_start(i2c, addr, I2C_MASTER_TRANSMIT, len) != 0) {
        return -1;
    }

    for (uint16_t i = 0U; i < len; i++) {
        if (bsp_i2c_wait_flag(i2c, I2C_FLAG_TBE, RESET) != 0) {
            return -1;
        }
        i2c_data_transmit(i2c, (uint32_t)buf[i]);
    }

    if (bsp_i2c_wait_flag(i2c, I2C_FLAG_TC, RESET) != 0) {
        return -1;
    }

    return (int)len;
}

int bsp_i2c_read(uint32_t i2c, uint8_t addr, uint8_t *buf, uint16_t len)
{
    if (len == 0U) {
        return 0;
    }
    if (len > 255U) {
        len = 255U;
    }

    if (bsp_i2c_master_start(i2c, addr, I2C_MASTER_RECEIVE, len) != 0) {
        return -1;
    }

    for (uint16_t i = 0U; i < len; i++) {
        if (bsp_i2c_wait_flag(i2c, I2C_FLAG_RBNE, RESET) != 0) {
            return -1;
        }
        buf[i] = (uint8_t)i2c_data_receive(i2c);
    }

    if (bsp_i2c_wait_flag(i2c, I2C_FLAG_TC, RESET) != 0) {
        return -1;
    }

    return (int)len;
}

int bsp_i2c_mem_write(uint32_t i2c, uint8_t addr, uint16_t mem_addr, uint8_t mem_addr_size,
                      const uint8_t *buf, uint16_t len)
{
    uint8_t tmp[2];

    if (mem_addr_size == 1U) {
        tmp[0] = (uint8_t)mem_addr;
        if (bsp_i2c_write(i2c, addr, tmp, 1U) < 0) {
            return -1;
        }
    } else {
        tmp[0] = (uint8_t)(mem_addr >> 8U);
        tmp[1] = (uint8_t)(mem_addr & 0xFFU);
        if (bsp_i2c_write(i2c, addr, tmp, 2U) < 0) {
            return -1;
        }
    }

    return bsp_i2c_write(i2c, addr, buf, len);
}

int bsp_i2c_mem_read(uint32_t i2c, uint8_t addr, uint16_t mem_addr, uint8_t mem_addr_size,
                     uint8_t *buf, uint16_t len)
{
    uint8_t tmp[2];

    if (mem_addr_size == 1U) {
        tmp[0] = (uint8_t)mem_addr;
        if (bsp_i2c_write(i2c, addr, tmp, 1U) < 0) {
            return -1;
        }
    } else {
        tmp[0] = (uint8_t)(mem_addr >> 8U);
        tmp[1] = (uint8_t)(mem_addr & 0xFFU);
        if (bsp_i2c_write(i2c, addr, tmp, 2U) < 0) {
            return -1;
        }
    }

    return bsp_i2c_read(i2c, addr, buf, len);
}
