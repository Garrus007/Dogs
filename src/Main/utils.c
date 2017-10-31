#include "utils.h"

RCC_ClocksTypeDef clock;

void UtilsInit(void)
{
  RCC_GetClocksFreq(&clock);
}

void WtfDelay(void)
{
  for(int i = 0; i<100000; i++){}
}


// Переводит такты в микросекунды
float DurationUs(uint32_t duration)
{
  return duration / (float)clock.SYSCLK_Frequency * 1000000;
}

float DurationMs(uint32_t duration)
{
  return duration / (float)clock.SYSCLK_Frequency * 1000;
}

// Создает тестовый сигнал из синусов
void CreateSignal(float* fft_buffer, int size)
{
  int fs = 1000;            // sampling rate
  float T = 1/(float)fs;    // sampling period
  float t;
  float y;
  float pi = 3.14159;
  
  for(int i = 0; i<size*2; i+=2)
  {
    t = i / 2 * T;
    y = sinf(2 * pi * t * 10) + 2 * sinf(2*pi*t*400);
    
    fft_buffer[i] = y;
    fft_buffer[i+1] = 0;
  }
  
}
