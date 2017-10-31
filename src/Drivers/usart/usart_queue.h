#ifndef __USARTQUEUE_H__
#define __USARTQUEUE_H__

#include <stdint.h>
#include <stdbool.h>


#define STDIO_SEND_BUFFER_SIZE 500

typedef struct
{
    uint8_t Buffer[STDIO_SEND_BUFFER_SIZE];
    uint8_t Head;
    uint8_t Tail;
} Queue;



// Инициализация
void Queue_Init(Queue* queue);

// Добавление в очередь
void Queue_Push(Queue* queue, uint8_t byte);

// Извлечение из очереди
uint8_t Queue_Pull(Queue* queue);

// Пуста ли очередь
bool Queue_IsEmpty(Queue* queue);


#endif
