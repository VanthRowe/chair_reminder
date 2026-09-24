#include "device_led.h"
#include "bsp_pwm.h"
#include "bsp_timer.h"

#define PWM_FREQ_HZ        1000U
#define TIMER6_TICK_HZ     1000000U  /* TIMER6 延时基准：1MHz，即 1us 一个 tick */
#define TIMER6_TICKS_PER_MS  (TIMER6_TICK_HZ / 1000U)
#define BREATH_STEPS       32U
#define BREATH_STEP_MS     40U
#define PEAK_HOLD_MS       800U
#define VALLEY_HOLD_MS     1200U

/* 伽马 2.0 占空比查找表（0~100） */
static const uint8_t breath_duty[BREATH_STEPS] = {
    0U,  0U,  0U,  1U,  2U,  3U,  4U,  5U,
    7U,  8U,  10U, 13U, 15U, 18U, 20U, 23U,
    27U, 30U, 34U, 38U, 42U, 46U, 50U, 55U,
    60U, 65U, 70U, 76U, 82U, 87U, 94U, 100U
};

static void breath_set_duty(uint8_t duty)
{
    bsp_pwm_set_duty(TIMER1, TIMER_CH_1, duty);
    bsp_pwm_set_duty(TIMER1, TIMER_CH_2, duty);
    bsp_pwm_set_duty(TIMER1, TIMER_CH_3, duty);
}

static void breath_delay_ms(uint32_t ms)
{
    for (uint32_t i = 0U; i < ms; i++) {
        bsp_timer_delay_ticks(TIMER6, TIMER6_TICKS_PER_MS);
    }
}

void device_led_init(void)
{
    /* PA1=R(TIMER1_CH1, AF1), PA2=G(TIMER1_CH2, AF1), PA3=B(TIMER1_CH3, AF1) */
    const bsp_pwm_cfg_t pwm_r = {
        TIMER1, TIMER_CH_1, GPIOA, GPIO_PIN_1, GPIO_AF_1,
        TIMER_OC_POLARITY_HIGH, PWM_FREQ_HZ, 0U
    };

    bsp_pwm_init(&pwm_r);
    bsp_pwm_channel_add(TIMER1, TIMER_CH_2, GPIOA, GPIO_PIN_2, GPIO_AF_1, TIMER_OC_POLARITY_HIGH);
    bsp_pwm_channel_add(TIMER1, TIMER_CH_3, GPIOA, GPIO_PIN_3, GPIO_AF_1, TIMER_OC_POLARITY_HIGH);

    bsp_timer_init(TIMER6, TIMER6_TICK_HZ);
}

void device_led_breath_run(void)
{
    /* 渐亮 */
    for (uint8_t step = 0U; step < BREATH_STEPS; step++) {
        breath_set_duty(breath_duty[step]);
        breath_delay_ms(BREATH_STEP_MS);
    }

    breath_delay_ms(PEAK_HOLD_MS);

    /* 渐暗 */
    for (int8_t step = (int8_t)(BREATH_STEPS - 1U); step >= 0; step--) {
        breath_set_duty(breath_duty[(uint8_t)step]);
        breath_delay_ms(BREATH_STEP_MS);
    }

    breath_delay_ms(VALLEY_HOLD_MS);
}
