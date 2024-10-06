/*
 * CubeTask.cpp
 *
 *  Created on: Oct 18, 2023
 *      Author: Chris
 */

#include "CubeTask.hpp"
#include "UARTDriver.hpp"

/**
 * @brief Initializes Cube task with the RTOS scheduler
*/
void CubeTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize UART task twice");

    // Start the task
    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)CubeTask::RunTask,
            (const char*)"CUBETask",
            (uint16_t)UART_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)UART_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    //Ensure creation succeded
    SOAR_ASSERT(rtValue == pdPASS, "CUBETask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the Cube Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
*/
void CubeTask::Run(void * pvParams)
{
    //UART Task loop
    while(1) {
        Command cm;

        //Wait forever for a command
        qEvtQueue->ReceiveWait(cm);

        //Process the command
        HandleCommand(cm);
    }
}

/**
 * @brief HandleCommand handles any command passed to the Cube task primary event queue. Responsible for
 *           handling all commands, even if unsupported. (Unexpected commands must still be reset)
 * @param cm Reference to the command object to handle
*/
void CubeTask::HandleCommand(Command& cm)
{
    //Switch for the GLOBAL_COMMAND
    switch (cm.GetCommand()) {
    case DATA_COMMAND: {
        //Switch for task specific command within DATA_COMMAND
        switch (cm.GetTaskCommand()) {
        case CUBE_TASK_COMMAND_SEND_DEBUG:
#ifndef DISABLE_DEBUG
                DEFAULT_DEBUG_UART_DRIVER->Transmit(cm.GetDataPointer(), cm.GetDataSize());
#endif
            break;
        default:
            SOAR_PRINT("CUBETask - Received Unsupported DATA_COMMAND {%d}\n", cm.GetTaskCommand());
            break;
        }
        break;
    }
    default:
        SOAR_PRINT("CUBETask - Received Unsupported Command {%d}\n", cm.GetCommand());
        break;
    }

    //No matter what we happens, we must reset allocated data
    cm.Reset();
}





