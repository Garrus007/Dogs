#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>
#include "usart_queue.h"

typedef void (*CallbackHandler)(int16_t, int16_t, int16_t);

void UartInit(uint32_t baud, CallbackHandler handler);
void UartSend(uint8_t* data, uint8_t length);
int  sendchar(int ch);

#endif
