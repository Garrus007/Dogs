#include "usart.h"

#include "stm32f4xx_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f4xx_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "stm32f4xx_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

Queue _sendQueue;

void UartInit(uint32_t baud)
{
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
  //Настройка пинов USART
	GPIO_InitTypeDef gpioInit;
  
  //TX - PA9
  gpioInit.GPIO_Mode = GPIO_Mode_AF;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInit.GPIO_Pin = GPIO_Pin_9;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpioInit);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  
	//Настройка USART
	USART_InitTypeDef usartInit;
	usartInit.USART_BaudRate = baud;
	usartInit.USART_WordLength = USART_WordLength_8b;
	usartInit.USART_StopBits = USART_StopBits_1;
	usartInit.USART_Parity = USART_Parity_No;
	usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_DeInit(USART1);
	USART_Init(USART1, &usartInit);
	USART_Cmd(USART1, ENABLE);
	
  
  Queue_Init(&_sendQueue);

	//Настройка прерываний USART	
	NVIC_InitTypeDef init;
	init.NVIC_IRQChannel = USART1_IRQn;
	init.NVIC_IRQChannelPreemptionPriority  = 11;
	init.NVIC_IRQChannelSubPriority = 0;
	init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&init);
}


void UartSend(uint8_t* data, uint8_t length)
{    
    for(uint8_t i = 0; i<length; i++)
    {
        Queue_Push(&_sendQueue, data[i]);
    }
        
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}


int  sendchar(int ch)
{
  Queue_Push(&_sendQueue, (uint8_t)ch);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  return ch;
}

void USART1_IRQHandler()
{
  if(USART_GetITStatus(USART1, USART_IT_TXE)!=RESET)
	{
        // Передача
        
        USART_ClearITPendingBit(USART1, USART_IT_TXE);
        
        if(Queue_IsEmpty(&_sendQueue))
        {
            // Очередь заполнена
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);  
            return;
        }
        
        uint8_t c = Queue_Pull(&_sendQueue);
        USART_SendData(USART1, c);
    }
}
