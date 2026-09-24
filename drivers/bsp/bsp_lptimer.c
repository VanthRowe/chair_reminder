#include "bsp_lptimer.h"

static uint32_t pwm_counts;   /* PWM 周期总计数（CAR+1），占空比换算用 */

static void bsp_lptimer_gpio_clock_enable(uint32_t port)
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

void bsp_lptimer_pwm_init(uint32_t gpio_port, uint32_t gpio_pin, uint32_t af_num,
                          uint32_t freq_hz, uint8_t duty)
{
    bsp_lptimer_gpio_clock_enable(gpio_port);
    gpio_mode_set(gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, gpio_pin);
    gpio_output_options_set(gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_pin);
    if (af_num != 0U) {
        gpio_af_set(gpio_port, af_num, gpio_pin);
    }

    rcu_periph_clock_enable(RCU_LPTIMER);
    lptimer_deinit(LPTIMER);

    lptimer_parameter_struct initpara;
    lptimer_struct_para_init(&initpara);
    initpara.clocksource    = LPTIMER_INTERNALCLK;
    initpara.prescaler      = LPTIMER_PSC_1;
    initpara.triggermode    = LPTIMER_TRIGGER_SOFTWARE;
    initpara.outputpolarity = LPTIMER_OUTPUT_NOTINVERTED;
    initpara.outputmode     = LPTIMER_OUTPUT_PWMORSINGLE;
    initpara.countersource  = LPTIMER_COUNTER_INTERNAL;
    lptimer_init(LPTIMER, &initpara);

    /* LPTIMERSEL 复位默认 00：内核时钟 = CK_APB2 */
    pwm_counts = rcu_clock_freq_get(CK_APB2) / freq_hz;
    if (pwm_counts < 2U) {
        pwm_counts = 2U;
    }

    /* PWM：CNT 计到 CMPV 置位、计到 CAR 清零，高电平计数 = counts - cmp */
    lptimer_countinue_start(LPTIMER, pwm_counts - 1U,
                            pwm_counts - (pwm_counts * duty) / 100U);
}

void bsp_lptimer_set_duty(uint8_t duty)
{
    uint32_t cmp;

    if (duty > 100U) {
        duty = 100U;
    }
    cmp = pwm_counts - (pwm_counts * duty) / 100U;
    lptimer_compare_value_config(LPTIMER, cmp);
}
