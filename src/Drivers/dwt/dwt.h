#ifndef __DWT_H__
#define __DWT_H__

#include <stdint.h>
#include <stm32f4xx.h>

void DwtInit(void);
void DwtClearTick(void);
uint32_t DwtGetTick(void);

#endif
