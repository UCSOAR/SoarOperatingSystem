#ifndef WATCHDOGTASK_HPP
#define WATCHDOGTASK_HPP

#include "stm32h7xx_hal.h"

class WatchdogTask
{
public:
    static WatchdogTask& Inst();

    void InitTask();
    void Pet();
    const char* GetLastTaskToPet();

private:
    WatchdogTask() = default;
    static void TaskEntry(void* argument);
};

#endif