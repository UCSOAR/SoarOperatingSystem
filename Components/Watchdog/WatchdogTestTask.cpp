#include "WatchdogTestTask.hpp"
#include "WatchdogTask.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "SystemDefines.hpp"

enum WatchdogTestMode
{
    TEST_STOP_PETTING = 0,
    TEST_FREEZE_TASK  = 1
};

// Change this to switch tests
static WatchdogTestMode testMode = TEST_FREEZE_TASK;

static void TaskEntry(void* argument)
{
    int counter = 0;

    SOAR_PRINT("Watchdog Test Task Started, mode: %d\n", (int)testMode);

    while (1)
    {
        SOAR_PRINT("TEST TASK RUNNING [counter=%d]\n", counter);

        switch (testMode)
        {
            case TEST_STOP_PETTING:
            {
                if (counter < 5)
                {
                    SOAR_PRINT("Test1: Petting watchdog (%d/5)\n", counter + 1);
                    WatchdogTask::Inst().Pet();
                }
                else
                {
                    // Stop petting — watchdog should reset the board
                    SOAR_PRINT("Test1: STOPPED petting, board should reset soon...\n");

                }
                break;
            }

            case TEST_FREEZE_TASK:
            {
                if (counter < 5)
                {
                    SOAR_PRINT("Test2: Petting watchdog (%d/5)\n", counter + 1);
                    WatchdogTask::Inst().Pet();
                }
                else if (counter == 5)
                {
                    SOAR_PRINT("Test2: FREEZING TASK, board should reset soon...\n");
                    while (1);  // Simulate frozen task, watchdog will trigger
                }
                break;
            }
        }

        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

WatchdogTestTask& WatchdogTestTask::Inst()
{
    static WatchdogTestTask instance;
    return instance;
}

void WatchdogTestTask::InitTask()
{
    xTaskCreate(TaskEntry, "WDT_Test", 512, NULL, tskIDLE_PRIORITY + 2, NULL);
}
