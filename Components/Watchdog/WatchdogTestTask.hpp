/*
 * WatchdogTestTask.hpp
 *
 *  Created on: May 1, 2026
 *      Author: Owner
 */

#ifndef WATCHDOG_TEST_TASK_HPP
#define WATCHDOG_TEST_TASK_HPP

class WatchdogTestTask
{
public:
    static WatchdogTestTask& Inst();
    void InitTask();

private:
    WatchdogTestTask() = default;
};

#endif