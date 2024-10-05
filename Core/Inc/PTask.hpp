/**
 ******************************************************************************
 * File Name          : PTask.hpp
 * Description        : Priority task contains the core component for all tasks,
 *                      with a priority-based event queue.
 ******************************************************************************
*/
#ifndef CUBE_INCLUDE_CORE_PRIORITY_TASK_HPP
#define CUBE_INCLUDE_CORE_PRIORITY_TASK_HPP
/* Includes ------------------------------------------------------------------*/
#include <cmsis_os.h>
#include "PQueue.hpp"
#include "SystemDefines.hpp"

/* Macros and Constants --------------------------------------------------*/
constexpr uint16_t DEFAULT_PQUEUE_DEPTH = 10;

/* Enums -----------------------------------------------------------------*/

/* Class -----------------------------------------------------------------*/
class PTask<const size_t DEPTH = DEFAULT_PQUEUE_DEPTH> {
public:
    //Constructors
    PTask(void) {
        rtTaskHandle_ = nullptr;
        qEvtQueue_ = new PQueue<Command,DEPTH>();
    }

    virtual void InitTask() = 0; 

    PQueue<Command, DEPTH>* GetEventQueue() const { return qEvtQueue_; }
    void SendCommand(Command cmd, uint8_t priority) { qEvtQueue->Send(cmd, priority); }
    void SendCommandReference(Command& cmd, uint8_t priority) { qEvtQueue->Send(cmd, priority); }

protected:
    //RTOS
    TaskHandle_t rtTaskHandle_;   // RTOS Task Handle

    //Task structures
    PQueue<Command,DEPTH>* qEvtQueue_;    // Task event queue
};

#endif /* CUBE_INCLUDE_CORE_PRIORITY_TASK_HPP */
