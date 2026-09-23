#include "bsp_fwdgt.h"
#include "gd32l23x.h"

#define BSP_FWDGT_IRC32K_FREQ  32000U

static const uint8_t fwdgt_prescaler[] = {
    FWDGT_PSC_DIV4,
    FWDGT_PSC_DIV8,
    FWDGT_PSC_DIV16,
    FWDGT_PSC_DIV32,
    FWDGT_PSC_DIV64,
    FWDGT_PSC_DIV128,
    FWDGT_PSC_DIV256
};

static const uint16_t fwdgt_div_value[] = {4U, 8U, 16U, 32U, 64U, 128U, 256U};

void bsp_fwdgt_init(uint32_t timeout_ms)
{
    uint16_t reload = 0U;
    uint8_t prescaler = FWDGT_PSC_DIV256;
    uint32_t div = 256U;

    for (uint8_t i = 0U; i < sizeof(fwdgt_div_value) / sizeof(fwdgt_div_value[0]); i++) {
        div = fwdgt_div_value[i];
        reload = (uint16_t)((timeout_ms * (BSP_FWDGT_IRC32K_FREQ / 1000U)) / div);
        if (reload <= 4095U && reload > 0U) {
            prescaler = fwdgt_prescaler[i];
            break;
        }
    }

    if (reload == 0U) {
        reload = 4095U;
        prescaler = FWDGT_PSC_DIV256;
    } else if (reload > 4095U) {
        reload = 4095U;
    }

    fwdgt_config(reload, prescaler);
    fwdgt_enable();
}

void bsp_fwdgt_feed(void)
{
    fwdgt_counter_reload();
}
