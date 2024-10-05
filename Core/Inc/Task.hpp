/**
 ******************************************************************************
 * File Name          : Task.hpp
 * Description        : Task contains the core component base class for all tasks.
 ******************************************************************************
*/
#ifndef CUBE_INCLUDE_SOAR_CORE_TASK_H
#define CUBE_INCLUDE_SOAR_CORE_TASK_H
/* Includes ------------------------------------------------------------------*/
#include <cmsis_os.h>
#include <Core/Inc/Queue.hpp>

/* Macros --------------------------------------------------------------------*/

/* Enums -----------------------------------------------------------------*/

/* Class -----------------------------------------------------------------*/

class Task {
public:
    //Constructors
    Task(void);
    Task(uint16_t depth);

    virtual void InitTask() = 0;

    Queue* GetEventQueue() const { return qEvtQueue; }
    void SendCommand(Command cmd) { qEvtQueue->Send(cmd); }
    void SendCommandReference(Command& cmd) { qEvtQueue->Send(cmd); }

protected:
    //RTOS
    TaskHandle_t rtTaskHandle;        // RTOS Task Handle

    //Task structures
    Queue* qEvtQueue;    // Task event queue
};

#endif /* CUBE_INCLUDE_SOAR_CORE_TASK_H */
