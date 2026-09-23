#include "bsp_adc.h"

static void bsp_adc_gpio_clock_enable(uint32_t port)
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

void bsp_adc_init(uint8_t channel, uint32_t gpio_port, uint32_t gpio_pin)
{
    (void)channel;

    rcu_periph_clock_enable(RCU_ADC);
    bsp_adc_gpio_clock_enable(gpio_port);

    gpio_mode_set(gpio_port, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, gpio_pin);

    adc_deinit();
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    adc_resolution_config(ADC_RESOLUTION_12B);
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
    adc_external_trigger_config(ADC_ROUTINE_CHANNEL, DISABLE);
    adc_channel_length_config(ADC_ROUTINE_CHANNEL, 1U);

    adc_enable();
    adc_calibration_enable();
}

void bsp_adc_sample_time_config(uint8_t channel, uint32_t sample_time)
{
    adc_routine_channel_config(0U, channel, sample_time);
}

uint16_t bsp_adc_read(uint8_t channel)
{
    adc_flag_clear(ADC_FLAG_EOC);
    adc_routine_channel_config(0U, channel, ADC_SAMPLETIME_13POINT5);
    adc_software_trigger_enable(ADC_ROUTINE_CHANNEL);

    while (adc_flag_get(ADC_FLAG_EOC) == RESET) {
    }
    adc_flag_clear(ADC_FLAG_EOC);

    return adc_routine_data_read();
}
