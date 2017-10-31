#include <stm32f4xx_rcc.h>
#include <Drivers/usart/usart.h>
#include <Drivers/dwt/dwt.h>
#include <stdio.h>
#include "wavelet.h"
#include "utils.h"

int dataCount = 0;
uint16_t totalSteps  = 0;

void DataReceived(int16_t x, int16_t y, int16_t z)
{
  //printf("%d, %d, %d\r\n", x, y, z);
  waveletProcessNewData(x, y, z, 0);
  dataCount++;
  
  if(dataCount == 250)
  {
    DwtClearTick();
    uint32_t t0 = DwtGetTick();
    uint16_t currentSteps = waveletGetStepsCount();
    uint32_t duration = DwtGetTick() - t0;
    
    //printf("%d\n", currentSteps);
    printf("%d\t%f\n", currentSteps, DurationMs(duration));
    
    dataCount=0;
  }
}

int main()
{
  UartInit(256000, DataReceived);
  UtilsInit();
    
  DwtInit();
  DwtClearTick();
  
  
  while(1)
  {
  }
  
}
