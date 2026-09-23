#include "bsp_dac.h"

#define BSP_DAC_VREF_MV  3300U

static void bsp_dac_gpio_clock_enable(uint32_t port)
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

void bsp_dac_init(uint32_t gpio_port, uint32_t gpio_pin)
{
    rcu_periph_clock_enable(RCU_DAC);
    bsp_dac_gpio_clock_enable(gpio_port);

    gpio_mode_set(gpio_port, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, gpio_pin);

    dac_deinit(DAC0);
    dac_trigger_disable(DAC0, DAC_OUT0);
    dac_output_buffer_enable(DAC0, DAC_OUT0);
    dac_enable(DAC0, DAC_OUT0);
}

void bsp_dac_set_value(uint16_t value)
{
    if (value > 4095U) {
        value = 4095U;
    }
    dac_data_set(DAC0, DAC_OUT0, DAC_ALIGN_12B_R, value);
}

void bsp_dac_set_voltage(uint16_t mv)
{
    if (mv > BSP_DAC_VREF_MV) {
        mv = BSP_DAC_VREF_MV;
    }
    uint32_t value = ((uint32_t)mv * 4095U) / BSP_DAC_VREF_MV;
    bsp_dac_set_value((uint16_t)value);
}
