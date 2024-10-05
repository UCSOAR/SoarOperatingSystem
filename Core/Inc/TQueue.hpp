/**
 ******************************************************************************
 * File Name          : TQueue.hpp
 * Description        :
 *
 *    Queue is a wrapper for FreeRTOS Queues for use in C++.
 *    with an internal queue handle and queue depth.
 * 
 *    This is an alternative version of Queue.hpp which allows for a template argument,
 *    but due to not using a specific object type it does NOT offer Command class
 *    memory handling capabilities that the dedicated Queue does
 ******************************************************************************
*/
#ifndef CUBE_PLUSPLUS_INCLUDE_CUBE_CORE_TQUEUE_H
#define CUBE_PLUSPLUS_INCLUDE_CUBE_CORE_TQUEUE_H
/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "Command.hpp"
#include "CubeUtils.hpp"
#include "FreeRTOS.h"

/* Macros --------------------------------------------------------------------*/
#define DEFAULT_QUEUE_SEND_WAIT_TICKS (MS_TO_TICKS(15))    // We wait a max of 15ms to send to a queue

/* Constants -----------------------------------------------------------------*/

/* Class -----------------------------------------------------------------*/
template<typename T>
class TQueue {
public:
    //Constructors
    TQueue(void);
    TQueue(uint16_t depth);

    //Functions
    bool Send(T& item);
    bool SendFromISR(T& item);

    bool SendToFront(T& item);

    bool Receive(T& item, uint32_t timeout_ms = 0);
    bool ReceiveWait(T& item); //Blocks until an item is received

    //Getters
    uint16_t GetQueueMessageCount() const { return uxQueueMessagesWaiting(rtQueueHandle); }
    uint16_t GetQueueDepth() const { return queueDepth; }

    bool IsEmpty() const { return GetQueueMessageCount() == 0; }
    bool IsFull() const { return GetQueueMessageCount() == queueDepth; }

protected:
    //RTOS
    QueueHandle_t rtQueueHandle;    // RTOS Event Queue Handle
    
    //Data
    uint16_t queueDepth;            // Max queue depth
};

template<typename T>
TQueue<T>::TQueue(void)
{
    //Initialize RTOS Queue handle
    rtQueueHandle = xQueueCreate(DEFAULT_QUEUE_SIZE, sizeof(T));
    queueDepth = 0;
}

template<typename T>
TQueue<T>::TQueue(uint16_t depth)
{
    //Initialize RTOS Queue handle with given depth
    rtQueueHandle = xQueueCreate(depth, sizeof(T));
    queueDepth = depth;
}

template<typename T>
bool TQueue<T>::SendFromISR(T& item)
{
    if (xQueueSendFromISR(rtQueueHandle, &item, NULL) == pdPASS)
        return true;

    return false;
}

template<typename T>
bool TQueue<T>::SendToFront(T& item)
{
    if (xQueueSendToFront(rtQueueHandle, &item, DEFAULT_QUEUE_SEND_WAIT_TICKS) == pdPASS)
        return true;

    return false;
}

template<typename T>
bool TQueue<T>::Send(T& item)
{
    if (xQueueSend(rtQueueHandle, &item, DEFAULT_QUEUE_SEND_WAIT_TICKS) == pdPASS)
        return true;

    return false;
}

template<typename T>
bool TQueue<T>::Receive(T& item, uint32_t timeout_ms)
{
    if(xQueueReceive(rtQueueHandle, &item, MS_TO_TICKS(timeout_ms)) == pdTRUE) {
        return true;
    }
    return false;
}

template<typename T>
bool TQueue<T>::ReceiveWait(T& item)
{
    if (xQueueReceive(rtQueueHandle, &item, HAL_MAX_DELAY) == pdTRUE) {
        return true;
    }
    return false;
}

#endif /* CUBE_PLUSPLUS_INCLUDE_CUBE_CORE_TQUEUE_H */
