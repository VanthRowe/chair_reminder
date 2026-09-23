#ifndef BSP_WWDGT_H
#define BSP_WWDGT_H

#include <stdint.h>

void bsp_wwdgt_init(uint32_t timeout_ms);
void bsp_wwdgt_feed(void);

#endif /* BSP_WWDGT_H */
