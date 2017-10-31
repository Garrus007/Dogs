#include "usart_queue.h"

// Инициализация
void Queue_Init(Queue* queue)
{
   queue->Head = 0;
   queue->Tail = 0;
}

// Добавление в очередь
void Queue_Push(Queue* queue, uint8_t byte)
{
    queue->Buffer[queue->Head] = byte;
    queue->Head++;
    if(queue->Head>STDIO_SEND_BUFFER_SIZE)
        queue->Head = 0;
}

// Извлечение из очереди
uint8_t Queue_Pull(Queue* queue)
{
    uint8_t c = queue->Buffer[queue->Tail];
    queue->Tail++;
    if(queue->Tail > STDIO_SEND_BUFFER_SIZE)
        queue->Tail = 0;
    
    return c;
}

// Пуста ли очередь
bool Queue_IsEmpty(Queue* queue)
{
    return queue->Head == queue->Tail;
}
