#ifndef BSP_FWDGT_H
#define BSP_FWDGT_H

#include <stdint.h>

void bsp_fwdgt_init(uint32_t timeout_ms);
void bsp_fwdgt_feed(void);

#endif /* BSP_FWDGT_H */
