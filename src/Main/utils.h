#ifndef __UTILS_H__
#define __UTILS_H__

#include <stm32f4xx_rcc.h>
#include <math.h>

void UtilsInit(void);
void WtfDelay(void);
float DurationUs(uint32_t duration);
float DurationMs(uint32_t duration);
void CreateSignal(float* fft_buffer, int size);

#endif
