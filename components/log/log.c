#include "log.h"
#include "bsp_usart.h"
#include "bsp_timer.h"
#include <stdarg.h>
#include <stdio.h>

#define LOG_USART      USART0
#define LOG_BAUD       115200U
#define LOG_TX_PORT    GPIOA
#define LOG_TX_PIN     GPIO_PIN_9
#define LOG_RX_PORT    GPIOA
#define LOG_RX_PIN     GPIO_PIN_10
#define LOG_GPIO_AF    GPIO_AF_7

/* 时间戳使用独立的 TIMER2（TIMER1 给 LED PWM，TIMER6 给延时），1ms 一个 tick */
#define LOG_TS_TIMER   TIMER2
#define LOG_TS_FREQ_HZ 1000U

#define LOG_BUF_SIZE   128U

static log_level_t s_level = LOG_LEVEL_DEBUG;
static uint32_t s_ms = 0U;
static uint16_t s_last_tick = 0U;

static const char s_level_char[] = "DIWE";

/* 16 位定时器自由运行，用差值累加出 32 位毫秒时间戳 */
static uint32_t log_tick_ms(void)
{
    uint16_t now = (uint16_t)bsp_timer_counter_get(LOG_TS_TIMER);
    s_ms += (uint16_t)(now - s_last_tick);
    s_last_tick = now;
    return s_ms;
}

static void log_output(log_level_t level, const char *tag, const char *msg)
{
    char buf[LOG_BUF_SIZE];

    int n = snprintf(buf, sizeof(buf), "[%8lu ms] %c [%s] %s\r\n",
                     (unsigned long)log_tick_ms(), s_level_char[(uint8_t)level], tag, msg);
    if (n <= 0) {
        return;
    }

    uint16_t len = ((uint32_t)n < sizeof(buf)) ? (uint16_t)n : (uint16_t)(sizeof(buf) - 1U);
    bsp_usart_send(LOG_USART, (const uint8_t *)buf, len);
}

void log_init(void)
{
    const bsp_usart_cfg_t cfg = {
        LOG_USART, LOG_BAUD,
        LOG_TX_PORT, LOG_TX_PIN, LOG_GPIO_AF,
        LOG_RX_PORT, LOG_RX_PIN, LOG_GPIO_AF
    };

    bsp_usart_init(&cfg);
    bsp_timer_init(LOG_TS_TIMER, LOG_TS_FREQ_HZ);
    s_last_tick = (uint16_t)bsp_timer_counter_get(LOG_TS_TIMER);
}

void log_set_level(log_level_t level)
{
    s_level = level;
}

log_level_t log_get_level(void)
{
    return s_level;
}

void log_write(log_level_t level, const char *tag, const char *fmt, ...)
{
    if ((level < s_level) || (level >= LOG_LEVEL_NONE)) {
        return;
    }

    char msg[LOG_BUF_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    log_output(level, tag, msg);
}

void log_assert_failed(const char *expr, const char *file, uint32_t line)
{
    /* 断言不受日志阈值过滤，必须打印出来 */
    char msg[LOG_BUF_SIZE];
    snprintf(msg, sizeof(msg), "%s failed at %s:%lu", expr, file, (unsigned long)line);
    log_output(LOG_LEVEL_ERROR, "ASSERT", msg);

    while (1) {
    }
}
