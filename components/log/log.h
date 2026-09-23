#ifndef LOG_H
#define LOG_H

#include <stdint.h>

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_NONE
} log_level_t;

/* 初始化日志组件：配置调试串口（USART0 PA9/PA10 115200）和时间戳定时器 */
void log_init(void);

/* 设置/获取全局输出阈值，低于阈值的日志被丢弃 */
void log_set_level(log_level_t level);
log_level_t log_get_level(void);

void log_write(log_level_t level, const char *tag, const char *fmt, ...);
void log_assert_failed(const char *expr, const char *file, uint32_t line);

#define LOG_D(tag, ...) log_write(LOG_LEVEL_DEBUG, tag, __VA_ARGS__)
#define LOG_I(tag, ...) log_write(LOG_LEVEL_INFO,  tag, __VA_ARGS__)
#define LOG_W(tag, ...) log_write(LOG_LEVEL_WARN,  tag, __VA_ARGS__)
#define LOG_E(tag, ...) log_write(LOG_LEVEL_ERROR, tag, __VA_ARGS__)

#define LOG_ASSERT(cond)                                  \
    do {                                                  \
        if (!(cond)) {                                    \
            log_assert_failed(#cond, __FILE__, __LINE__); \
        }                                                 \
    } while (0)

#endif /* LOG_H */
