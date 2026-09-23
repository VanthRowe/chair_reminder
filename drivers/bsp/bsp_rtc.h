#ifndef BSP_RTC_H
#define BSP_RTC_H

#include <stdint.h>

/* 24-hour format, binary values */
typedef struct {
    uint16_t year;   /* 2000 ~ 2099 */
    uint8_t  month;  /* 1 ~ 12 */
    uint8_t  date;   /* 1 ~ 31 */
    uint8_t  wday;   /* 1 ~ 7 */
    uint8_t  hour;   /* 0 ~ 23 */
    uint8_t  minute; /* 0 ~ 59 */
    uint8_t  second; /* 0 ~ 59 */
} bsp_rtc_datetime_t;

int bsp_rtc_init(void);

void bsp_rtc_set(const bsp_rtc_datetime_t *dt);
void bsp_rtc_get(bsp_rtc_datetime_t *dt);

#endif /* BSP_RTC_H */
