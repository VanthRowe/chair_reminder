#include "device_led.h"
#include "log.h"

int main(void)
{
    log_init();
    device_led_init();

    LOG_I("MAIN", "chair_reminder start");
    LOG_D("MAIN", "debug level %d", log_get_level());

    while (1) {
        device_led_breath_run();
        LOG_D("LED", "breath cycle done");
    }
}
