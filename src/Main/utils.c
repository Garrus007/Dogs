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