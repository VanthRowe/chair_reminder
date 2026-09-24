#ifndef DEVICE_BUZZER_H
#define DEVICE_BUZZER_H

#include <stdint.h>

void device_buzzer_init(void);

/* 短鸣 / 长鸣 / 指定时长鸣响（含渐强渐弱），阻塞返回 */
void device_buzzer_short(void);
void device_buzzer_long(void);
void device_buzzer_beep(uint32_t duration_ms);

/* 不定时鸣响：start 渐强起鸣后保持，由上层调用 stop 渐弱结束 */
void device_buzzer_start(void);
void device_buzzer_stop(void);

/* 连续鸣响 times 次，on_ms 单次时长，off_ms 间隔静音时长 */
void device_buzzer_beep_times(uint8_t times, uint32_t on_ms, uint32_t off_ms);

#endif /* DEVICE_BUZZER_H */
