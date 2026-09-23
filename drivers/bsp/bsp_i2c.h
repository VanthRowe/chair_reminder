#ifndef BSP_I2C_H
#define BSP_I2C_H

#include <stdint.h>
#include "gd32l23x.h"

typedef enum {
    BSP_I2C_SPEED_100K = 0,
    BSP_I2C_SPEED_400K
} bsp_i2c_speed_t;

typedef struct {
    uint32_t i2c;

    uint32_t scl_port;
    uint32_t scl_pin;
    uint32_t scl_af;

    uint32_t sda_port;
    uint32_t sda_pin;
    uint32_t sda_af;

    bsp_i2c_speed_t speed;
} bsp_i2c_cfg_t;

void bsp_i2c_init(const bsp_i2c_cfg_t *cfg);

int bsp_i2c_write(uint32_t i2c, uint8_t addr, const uint8_t *buf, uint16_t len);
int bsp_i2c_read(uint32_t i2c, uint8_t addr, uint8_t *buf, uint16_t len);

int bsp_i2c_mem_write(uint32_t i2c, uint8_t addr, uint16_t mem_addr, uint8_t mem_addr_size,
                      const uint8_t *buf, uint16_t len);
int bsp_i2c_mem_read(uint32_t i2c, uint8_t addr, uint16_t mem_addr, uint8_t mem_addr_size,
                     uint8_t *buf, uint16_t len);

#endif /* BSP_I2C_H */
