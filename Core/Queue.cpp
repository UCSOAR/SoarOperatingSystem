/**
 ******************************************************************************
 * File Name          : Queue.cpp
 * Description        : Implementation of the Queue class
 ******************************************************************************
*/
#include <Core/Inc/Command.hpp>
#include <Core/Inc/Queue.hpp>

#include "SystemDefines.hpp"
#include "FreeRTOS.h"
#include "task.h"


/**
 * @brief Constructor for the Queue class uses DEFAULT_QUEUE_SIZE for queue depth
*/
Queue::Queue(void)
{
    //Initialize RTOS Queue handle
    rtQueueHandle = xQueueCreate(DEFAULT_QUEUE_SIZE, sizeof(Command));
    queueDepth = 0;
}

/**
 * @brief Constructor with depth for the Queue class
 * @param depth Queue depth
*/
Queue::Queue(uint16_t depth)
{
    //Initialize RTOS Queue handle with given depth
    rtQueueHandle = xQueueCreate(depth, sizeof(Command));
    queueDepth = depth;
}

/**
 * @brief Sends a command object to the queue, safe to call from ISR
 * @param command Command object reference to send
 * @return true on success, false on failure (queue full)
*/
bool Queue::SendFromISR(Command& command)
{
    //Note: There NULL param here could be used to wake a task right after after exiting the ISR
    if (xQueueSendFromISR(rtQueueHandle, &command, NULL) == pdPASS)
        return true;

    command.Reset();

    return false;
}

/**
 * @brief Sends a command object to the front of the queue, use for high priority commands
 * @param command Command object reference to send
 * @return true on success, false on failure (queue full)
 */
bool Queue::SendToFront(Command& command)
{
    //Send to the back of the queue
    if (xQueueSendToFront(rtQueueHandle, &command, DEFAULT_QUEUE_SEND_WAIT_TICKS) == pdPASS)
        return true;

    SOAR_PRINT("Could not send data to front of queue!\n");
    command.Reset();

    return false;
}

/**
 * @brief Sends a command object to the queue (sends to back of queue in FIFO order)
 * @param command Command object reference to send
 * @param reportFull If true (default), prints an error message if the queue is full
 * @return true on success, false on failure (queue full)
 * 
 * //TODO: It may be possible to have this automatically set the command to not free data externally 
 * as we've "passed" control of the data over, which might let us use a destructor to free the data  
*/
bool Queue::Send(Command& command, bool reportFull)
{
    return Send(command, reportFull, DEFAULT_QUEUE_SEND_WAIT_TICKS);
}

/**
 * @brief Same as Send(), with the time to wait for space. 0 never blocks: a full queue drops
 *        the command straight away (DataBroker publishes, so a slow subscriber can't stall the sender)
*/
bool Queue::Send(Command& command, bool reportFull, uint32_t waitTicks)
{
    if (xQueueSend(rtQueueHandle, &command, waitTicks) == pdPASS)
        return true;

    // Debug: who sent, which queue (depth identifies the receiving task), what was dropped.
    // Only the first drop and then every 100th is printed, so a full queue can't flood the UART
    static uint32_t droppedTotal = 0;
    droppedTotal++;
    if (reportFull && (droppedTotal == 1 || droppedTotal % 100 == 0)) {
        SOAR_PRINT("Could not send data to queue! sender=%s queue=0x%08lx used=%u/%u cmd=%u taskCmd=%u size=%u t=%lums dropped=%lu\n",
                   pcTaskGetName(NULL), (unsigned long)(uintptr_t)rtQueueHandle,
                   (unsigned)uxQueueMessagesWaiting(rtQueueHandle), (unsigned)queueDepth,
                   (unsigned)command.GetCommand(), (unsigned)command.GetTaskCommand(),
                   (unsigned)command.GetDataSize(),
                   (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS),
                   (unsigned long)droppedTotal);
    }

    command.Reset();

    return false;
}

/**
 * @brief Polls queue with specific timeout, blocks for timeout_ms, returns null on no data
 * @param timeout_ms Time to block for
 * @param cm Command object to copy received data into
 * @return TRUE if we received a command, FALSE otherwise
*/
bool Queue::Receive(Command& cm, uint32_t timeout_ms)
{
    if(xQueueReceive(rtQueueHandle, &cm, MS_TO_TICKS(timeout_ms)) == pdTRUE) {
        return true;
    }
    return false;
}

/**
 * @brief Polls queue with specific timeout, blocks forever
 * @param cm Command object to copy received data into
 * @return TRUE if we received a command, FALSE otherwise (should rarely return false)
*/
bool Queue::ReceiveWait(Command& cm)
{
    if (xQueueReceive(rtQueueHandle, &cm, HAL_MAX_DELAY) == pdTRUE) {
        return true;
    }
    return false;
}
