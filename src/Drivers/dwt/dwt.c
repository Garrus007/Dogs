#include "dwt.h"

#define    DWT_CYCCNT    *(volatile unsigned long *)0xE0001004
#define    DWT_CONTROL   *(volatile unsigned long *)0xE0001000
#define    SCB_DEMCR     *(volatile unsigned long *)0xE000EDFC
  

void DwtInit(void)
{
  //разрешаем использовать счётчик
  SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  //обнуляем значение счётного регистра
	DWT_CYCCNT  = 0;
  //запускаем счётчик
	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk; 
}

void DwtClearTick(void)
{
  DWT_CYCCNT = 0;
}

uint32_t DwtGetTick(void)
{
  return DWT->CYCCNT;
}
