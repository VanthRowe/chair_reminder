#include "bsp_rtc.h"
#include "gd32l23x.h"

#define BSP_RTC_TIMEOUT  0xFFFFF0U

static uint8_t bsp_bin2bcd(uint8_t bin)
{
    return (uint8_t)(((bin / 10U) << 4U) | (bin % 10U));
}

static uint8_t bsp_bcd2bin(uint8_t bcd)
{
    return (uint8_t)(((bcd >> 4U) * 10U) + (bcd & 0x0FU));
}

static uint8_t bsp_rtc_year_bcd(uint16_t year)
{
    uint8_t y = (uint8_t)(year % 100U);
    return bsp_bin2bcd(y);
}

static int bsp_rtc_wait_lxtal(void)
{
    uint32_t timeout = BSP_RTC_TIMEOUT;

    rcu_osci_on(RCU_LXTAL);
    while ((rcu_flag_get(RCU_FLAG_LXTALSTB) == RESET) && (timeout > 0U)) {
        timeout--;
    }

    return (timeout > 0U) ? 0 : -1;
}

static void bsp_rtc_enter_init(void)
{
    rtc_init_mode_enter();
}

static void bsp_rtc_exit_init(void)
{
    rtc_init_mode_exit();
}

int bsp_rtc_init(void)
{
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    if (bsp_rtc_wait_lxtal() != 0) {
        return -1;
    }

    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    rcu_periph_clock_enable(RCU_RTC);

    bsp_rtc_enter_init();

    rtc_parameter_struct rtc_initpara;
    rtc_initpara.year           = 0x20U;
    rtc_initpara.month          = RTC_JAN;
    rtc_initpara.date           = 0x01U;
    rtc_initpara.day_of_week    = RTC_MONDAY;
    rtc_initpara.hour           = 0U;
    rtc_initpara.minute         = 0x00U;
    rtc_initpara.second         = 0x00U;
    rtc_initpara.factor_asyn    = 127U;
    rtc_initpara.factor_syn     = 255U;
    rtc_initpara.am_pm          = RTC_AM;
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_init(&rtc_initpara);

    bsp_rtc_exit_init();

    rtc_register_sync_wait();

    return 0;
}

void bsp_rtc_set(const bsp_rtc_datetime_t *dt)
{
    rtc_parameter_struct rtc_initpara;

    rtc_initpara.year           = bsp_rtc_year_bcd(dt->year);
    rtc_initpara.month          = dt->month;
    rtc_initpara.date           = bsp_bin2bcd(dt->date);
    rtc_initpara.day_of_week    = dt->wday;
    rtc_initpara.hour           = bsp_bin2bcd(dt->hour);
    rtc_initpara.minute         = bsp_bin2bcd(dt->minute);
    rtc_initpara.second         = bsp_bin2bcd(dt->second);
    rtc_initpara.factor_asyn    = 127U;
    rtc_initpara.factor_syn     = 255U;
    rtc_initpara.am_pm          = RTC_AM;
    rtc_initpara.display_format = RTC_24HOUR;

    bsp_rtc_enter_init();
    rtc_init(&rtc_initpara);
    bsp_rtc_exit_init();
    rtc_register_sync_wait();
}

void bsp_rtc_get(bsp_rtc_datetime_t *dt)
{
    rtc_parameter_struct rtc_initpara;

    rtc_register_sync_wait();
    rtc_current_time_get(&rtc_initpara);

    dt->year   = (uint16_t)(2000U + bsp_bcd2bin(rtc_initpara.year));
    dt->month  = rtc_initpara.month;
    dt->date   = bsp_bcd2bin(rtc_initpara.date);
    dt->wday   = rtc_initpara.day_of_week;
    dt->hour   = bsp_bcd2bin((uint8_t)(rtc_initpara.hour & 0x3FU));
    dt->minute = bsp_bcd2bin(rtc_initpara.minute);
    dt->second = bsp_bcd2bin(rtc_initpara.second);
}
