#include "bsp_wwdgt.h"
#include "gd32l23x.h"

#define BSP_WWDGT_MIN_COUNTER  64U

static const uint32_t wwdgt_prescaler[] = {
    WWDGT_CFG_PSC_DIV1,
    WWDGT_CFG_PSC_DIV2,
    WWDGT_CFG_PSC_DIV4,
    WWDGT_CFG_PSC_DIV8,
    WWDGT_CFG_PSC_DIV16,
    WWDGT_CFG_PSC_DIV32,
    WWDGT_CFG_PSC_DIV64,
    WWDGT_CFG_PSC_DIV128
};

static const uint32_t wwdgt_div_value[] = {1U, 2U, 4U, 8U, 16U, 32U, 64U, 128U};

void bsp_wwdgt_init(uint32_t timeout_ms)
{
    uint32_t pclk1 = rcu_clock_freq_get(CK_APB1);
    uint32_t prescaler = WWDGT_CFG_PSC_DIV128;
    uint32_t div = 128U;
    uint32_t counter = 127U;

    for (uint8_t i = 0U; i < sizeof(wwdgt_div_value) / sizeof(wwdgt_div_value[0]); i++) {
        div = wwdgt_div_value[i];
        uint32_t ticks = (timeout_ms * (pclk1 / 1000U)) / (4096U * div);
        if (ticks > 0U && ticks <= (127U - BSP_WWDGT_MIN_COUNTER) && (ticks + BSP_WWDGT_MIN_COUNTER) <= 127U) {
            counter = ticks + BSP_WWDGT_MIN_COUNTER;
            prescaler = wwdgt_prescaler[i];
            break;
        }
    }

    wwdgt_config((uint16_t)counter, 127U, prescaler);
    wwdgt_enable();
}

void bsp_wwdgt_feed(void)
{
    wwdgt_counter_update(127U);
}
