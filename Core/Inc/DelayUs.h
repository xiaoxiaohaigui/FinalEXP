#ifndef __DELAY_US_H__
#define __DELAY_US_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Delay_Init(void);
void Delay_us(uint16_t nus);

#ifdef __cplusplus
}
#endif

#endif /* __DELAY_US_H__ */