#include "bsp_timer.h"

static void bsp_timer_clock_enable(uint32_t timer_periph)
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

static uint32_t bsp_timer_clock_get(uint32_t timer_periph)
{
    if (timer_periph == TIMER8) {
        return rcu_clock_freq_get(CK_APB2);
    }
    return rcu_clock_freq_get(CK_APB1);
}

static void bsp_timer_tick_config(uint32_t timer_periph, uint32_t tick_freq_hz)
{
    uint32_t timer_clk = bsp_timer_clock_get(timer_periph);
    uint16_t prescaler = 0U;

    if (tick_freq_hz > 0U && tick_freq_hz <= timer_clk) {
        prescaler = (uint16_t)((timer_clk / tick_freq_hz) - 1U);
    }

    timer_parameter_struct initpara;
    timer_struct_para_init(&initpara);
    initpara.prescaler        = prescaler;
    initpara.period           = 0xFFFFU;
    initpara.alignedmode      = TIMER_COUNTER_EDGE;
    initpara.counterdirection = TIMER_COUNTER_UP;
    initpara.clockdivision    = TIMER_CKDIV_DIV1;
    timer_init(timer_periph, &initpara);
}

void bsp_timer_init(uint32_t timer_periph, uint32_t freq_hz)
{
    bsp_timer_clock_enable(timer_periph);
    bsp_timer_tick_config(timer_periph, freq_hz);
    timer_enable(timer_periph);
}

void bsp_timer_start(uint32_t timer_periph)
{
    timer_enable(timer_periph);
}

void bsp_timer_stop(uint32_t timer_periph)
{
    timer_disable(timer_periph);
}

uint32_t bsp_timer_counter_get(uint32_t timer_periph)
{
    return timer_counter_read(timer_periph);
}

void bsp_timer_delay_ticks(uint32_t timer_periph, uint32_t ticks)
{
    if (ticks == 0U) {
        return;
    }
    if (ticks > 0xFFFFU) {
        ticks = 0xFFFFU;
    }

    timer_disable(timer_periph);
    timer_counter_value_config(timer_periph, 0U);
    timer_autoreload_value_config(timer_periph, ticks);
    timer_event_software_generate(timer_periph, TIMER_EVENT_SRC_UPG);
    timer_flag_clear(timer_periph, TIMER_FLAG_UP);
    timer_enable(timer_periph);

    while (timer_flag_get(timer_periph, TIMER_FLAG_UP) == RESET) {
    }

    timer_disable(timer_periph);
    timer_flag_clear(timer_periph, TIMER_FLAG_UP);
}

void bsp_timer_delay_us(uint32_t timer_periph, uint32_t us)
{
    bsp_timer_tick_config(timer_periph, 1000000U);
    bsp_timer_delay_ticks(timer_periph, us);
}

void bsp_timer_delay_ms(uint32_t timer_periph, uint32_t ms)
{
    bsp_timer_tick_config(timer_periph, 1000U);
    bsp_timer_delay_ticks(timer_periph, ms);
}
