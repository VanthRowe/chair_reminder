#include "device_buzzer.h"
#include "bsp_pwm.h"
#include "bsp_timer.h"

/* 蜂鸣器 IO 配置：换 IO 口只需改这里（同步更新 AGENTS.md 资源分配表） */
/* PB14 = TIMER11_CH0, AF2（AF 映射以数据手册原表图为准） */
#define BUZZER_TIMER       TIMER11
#define BUZZER_CHANNEL     TIMER_CH_0
#define BUZZER_GPIO_PORT   GPIOB
#define BUZZER_GPIO_PIN    GPIO_PIN_14
#define BUZZER_GPIO_AF     GPIO_AF_2

#define BUZZER_FREQ_HZ     4000U   /* 无源蜂鸣器 PWM 驱动频率 */
#define BUZZER_MAX_DUTY    50U     /* 对称驱动最大占空比 */
#define BUZZER_FADE_MS     30U     /* 默认渐强/渐弱时长 */
#define BUZZER_FADE_STEPS  10U
#define BUZZER_SHORT_MS    1000U
#define BUZZER_LONG_MS     2000U

static void buzzer_delay_ms(uint32_t ms)
{
    /* 约定 TIMER6 固定 1MHz tick，禁止用 bsp_timer_delay_ms 重配 tick 频率 */
    for (uint32_t i = 0U; i < ms; i++) {
        bsp_timer_delay_ticks(TIMER6, 1000U);
    }
}

static void buzzer_fade(uint8_t duty_from, uint8_t duty_to, uint32_t fade_ms)
{
    uint32_t step_ms = fade_ms / BUZZER_FADE_STEPS;
    if (step_ms == 0U) {
        step_ms = 1U;
    }

    for (uint32_t i = 0U; i <= BUZZER_FADE_STEPS; i++) {
        uint8_t duty;
        if (duty_to >= duty_from) {
            duty = (uint8_t)(duty_from + ((uint32_t)(duty_to - duty_from) * i) / BUZZER_FADE_STEPS);
        } else {
            duty = (uint8_t)(duty_from - ((uint32_t)(duty_from - duty_to) * i) / BUZZER_FADE_STEPS);
        }
        bsp_pwm_set_duty(BUZZER_TIMER, BUZZER_CHANNEL, duty);
        buzzer_delay_ms(step_ms);
    }
}

void device_buzzer_init(void)
{
    const bsp_pwm_cfg_t cfg = {
        BUZZER_TIMER, BUZZER_CHANNEL, BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, BUZZER_GPIO_AF,
        TIMER_OC_POLARITY_HIGH, BUZZER_FREQ_HZ, 0U
    };

    bsp_pwm_init(&cfg);
    bsp_timer_init(TIMER6, 1000000U);
}

void device_buzzer_beep(uint32_t duration_ms)
{
    uint32_t fade_ms = BUZZER_FADE_MS;
    uint32_t hold_ms;

    if (duration_ms <= fade_ms * 2U) {
        fade_ms = duration_ms / 2U;
    }
    hold_ms = duration_ms - fade_ms * 2U;

    buzzer_fade(0U, BUZZER_MAX_DUTY, fade_ms);
    if (hold_ms > 0U) {
        buzzer_delay_ms(hold_ms);
    }
    buzzer_fade(BUZZER_MAX_DUTY, 0U, fade_ms);
}

void device_buzzer_short(void)
{
    device_buzzer_beep(BUZZER_SHORT_MS);
}

void device_buzzer_long(void)
{
    device_buzzer_beep(BUZZER_LONG_MS);
}

void device_buzzer_start(void)
{
    buzzer_fade(0U, BUZZER_MAX_DUTY, BUZZER_FADE_MS);
}

void device_buzzer_stop(void)
{
    buzzer_fade(BUZZER_MAX_DUTY, 0U, BUZZER_FADE_MS);
}

void device_buzzer_beep_times(uint8_t times, uint32_t on_ms, uint32_t off_ms)
{
    for (uint8_t i = 0U; i < times; i++) {
        device_buzzer_beep(on_ms);
        if ((i < (uint8_t)(times - 1U)) && (off_ms > 0U)) {
            buzzer_delay_ms(off_ms);
        }
    }
}
