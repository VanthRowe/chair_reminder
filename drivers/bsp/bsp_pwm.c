#include "bsp_pwm.h"

#define BSP_PWM_DEFAULT_RESOLUTION  1000U

static void bsp_pwm_clock_enable(uint32_t timer_periph)
{
    if (timer_periph == TIMER8) {
        rcu_periph_clock_enable(RCU_TIMER8);
    } else if (timer_periph == TIMER1) {
        rcu_periph_clock_enable(RCU_TIMER1);
    } else if (timer_periph == TIMER2) {
        rcu_periph_clock_enable(RCU_TIMER2);
    } else if (timer_periph == TIMER5) {
        rcu_periph_clock_enable(RCU_TIMER5);
    } else if (timer_periph == TIMER6) {
        rcu_periph_clock_enable(RCU_TIMER6);
    } else if (timer_periph == TIMER11) {
        rcu_periph_clock_enable(RCU_TIMER11);
    }
}

static void bsp_pwm_gpio_clock_enable(uint32_t port)
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

static uint32_t bsp_pwm_timer_clock_get(uint32_t timer_periph)
{
    if (timer_periph == TIMER8) {
        return rcu_clock_freq_get(CK_APB2);
    }
    return rcu_clock_freq_get(CK_APB1);
}

static uint16_t bsp_pwm_compute_period(uint32_t timer_clk, uint32_t freq_hz, uint16_t prescaler)
{
    uint32_t period = timer_clk / ((uint32_t)(prescaler + 1U) * freq_hz);
    if (period < 2U) {
        period = 2U;
    } else if (period > 0xFFFFU) {
        period = 0xFFFFU;
    }
    return (uint16_t)period;
}

static void bsp_pwm_compute(uint32_t timer_clk, uint32_t freq_hz,
                            uint16_t *prescaler, uint16_t *period)
{
    uint32_t target = timer_clk / freq_hz;

    /* 优先按默认分辨率计算 prescaler */
    if ((target / BSP_PWM_DEFAULT_RESOLUTION) > 0U &&
        (target / BSP_PWM_DEFAULT_RESOLUTION) <= 0x10000U) {
        *prescaler = (uint16_t)(target / BSP_PWM_DEFAULT_RESOLUTION - 1U);
        *period    = BSP_PWM_DEFAULT_RESOLUTION;
    } else if (target > 0U && target <= 0x10000U) {
        *prescaler = 0U;
        *period    = (uint16_t)(target - 1U);
    } else {
        /* 频率很低，取最大 period */
        *prescaler = 0xFFFFU;
        *period    = bsp_pwm_compute_period(timer_clk, freq_hz, 0xFFFFU);
    }
}

static void bsp_pwm_timebase_config(uint32_t timer_periph, uint32_t freq_hz)
{
    uint32_t timer_clk = bsp_pwm_timer_clock_get(timer_periph);
    uint16_t prescaler = 0U;
    uint16_t period    = 0U;

    bsp_pwm_compute(timer_clk, freq_hz, &prescaler, &period);

    timer_parameter_struct initpara;
    timer_struct_para_init(&initpara);
    initpara.prescaler        = prescaler;
    initpara.period           = period;
    initpara.alignedmode      = TIMER_COUNTER_EDGE;
    initpara.counterdirection = TIMER_COUNTER_UP;
    initpara.clockdivision    = TIMER_CKDIV_DIV1;
    timer_init(timer_periph, &initpara);
}

static void bsp_pwm_channel_config(uint32_t timer_periph, uint16_t channel, uint32_t oc_polarity)
{
    timer_oc_parameter_struct ocpara;
    timer_channel_output_struct_para_init(&ocpara);
    ocpara.outputstate = TIMER_CCX_ENABLE;
    ocpara.ocpolarity  = oc_polarity;
    timer_channel_output_config(timer_periph, channel, &ocpara);
    timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
}

static void bsp_pwm_pin_config(uint32_t gpio_port, uint32_t gpio_pin, uint32_t af_num)
{
    bsp_pwm_gpio_clock_enable(gpio_port);
    gpio_mode_set(gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, gpio_pin);
    gpio_output_options_set(gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_pin);
    if (af_num != 0U) {
        gpio_af_set(gpio_port, af_num, gpio_pin);
    }
}

static void bsp_pwm_gpio_config(const bsp_pwm_cfg_t *cfg)
{
    bsp_pwm_pin_config(cfg->gpio_port, cfg->gpio_pin, cfg->af_num);
}

static uint16_t bsp_pwm_period_get(uint32_t timer_periph)
{
    return (uint16_t)TIMER_CAR(timer_periph);
}

void bsp_pwm_init(const bsp_pwm_cfg_t *cfg)
{
    bsp_pwm_clock_enable(cfg->timer);
    timer_deinit(cfg->timer);

    bsp_pwm_gpio_config(cfg);
    bsp_pwm_timebase_config(cfg->timer, cfg->freq_hz);
    bsp_pwm_channel_config(cfg->timer, cfg->channel, cfg->oc_polarity);

    bsp_pwm_set_duty(cfg->timer, cfg->channel, cfg->duty);
    timer_enable(cfg->timer);
}

void bsp_pwm_channel_add(uint32_t timer, uint16_t channel,
                         uint32_t gpio_port, uint32_t gpio_pin, uint32_t af_num,
                         uint32_t oc_polarity)
{
    bsp_pwm_pin_config(gpio_port, gpio_pin, af_num);
    bsp_pwm_channel_config(timer, channel, oc_polarity);
    bsp_pwm_set_duty(timer, channel, 0U);
}

void bsp_pwm_start(uint32_t timer)
{
    timer_enable(timer);
}

void bsp_pwm_stop(uint32_t timer)
{
    timer_disable(timer);
}

void bsp_pwm_set_duty(uint32_t timer, uint16_t channel, uint8_t duty)
{
    uint16_t period = bsp_pwm_period_get(timer);
    uint32_t pulse  = ((uint32_t)period * duty) / 100U;
    if (pulse > 0xFFFFU) {
        pulse = 0xFFFFU;
    }
    timer_channel_output_pulse_value_config(timer, channel, pulse);
}

void bsp_pwm_set_freq(uint32_t timer, uint16_t channel, uint32_t oc_polarity,
                      uint32_t freq_hz, uint8_t duty)
{
    bsp_pwm_timebase_config(timer, freq_hz);
    bsp_pwm_channel_config(timer, channel, oc_polarity);
    bsp_pwm_set_duty(timer, channel, duty);
}
