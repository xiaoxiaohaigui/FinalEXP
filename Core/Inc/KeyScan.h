#ifndef __KEYSCAN_H
#define __KEYSCAN_H

#include <stdint.h>

#include "btn.h"

#define KEY_COUNT 4U
#define KEY_EVENT_NONE 0U
#define KEY_EVENT_CLICK 1U

void KeyScan_Init(void);
void KeyScan_ScanAll(void);

extern volatile uint8_t keyEvent[KEY_COUNT];

#endif /* __KEYSCAN_H */