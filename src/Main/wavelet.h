#ifndef __WAVELET_H__
#define __WAVELET_H__

#include <inttypes.h>
#include <arm_math.h>
#include <math.h>

#define MAX_BUFFER_SIZE 256 // must be 2^n

void waveletProcessNewData(int16_t x, int16_t y, int16_t z, uint16_t time);
uint16_t waveletGetStepsCount(void);

#endif
