#include "device_led.h"
#include "device_buzzer.h"
#include "log.h"

#define TEST_BEEP_MS       300U    /* 指定时长测试鸣响时长 */
#define TEST_BEEP_TIMES    3U      /* 连续鸣响测试次数 */
#define TEST_BEEP_ON_MS    100U
#define TEST_BEEP_OFF_MS   100U

int main(void)
{
    log_init();
    device_led_init();
    device_buzzer_init();

    LOG_I("MAIN", "chair_reminder start");

    while (1) {
        LOG_I("TEST", "short beep");
        device_buzzer_short();
        device_led_breath_run();

        LOG_I("TEST", "long beep");
        device_buzzer_long();
        device_led_breath_run();

        LOG_I("TEST", "beep %d ms", TEST_BEEP_MS);
        device_buzzer_beep(TEST_BEEP_MS);
        device_led_breath_run();

        LOG_I("TEST", "beep x%d (%d ms on, %d ms off)",
              TEST_BEEP_TIMES, TEST_BEEP_ON_MS, TEST_BEEP_OFF_MS);
        device_buzzer_beep_times(TEST_BEEP_TIMES, TEST_BEEP_ON_MS, TEST_BEEP_OFF_MS);
        device_led_breath_run();

        LOG_I("TEST", "indefinite start, hold one breath cycle, stop");
        device_buzzer_start();
        device_led_breath_run();
        device_buzzer_stop();
        device_led_breath_run();
    }
}
