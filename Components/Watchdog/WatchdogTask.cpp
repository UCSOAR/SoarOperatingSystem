#include "WatchdogTask.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <cstring>
#include "SystemDefines.hpp"


#if __has_include("stm32h7xx_hal.h")
#  include "stm32h7xx_hal.h"
#  define watchdogH7
#elif __has_include("stm32g4xx_hal.h")
#  include "stm32g4xx_hal.h"
#  define watchdogG4
#endif

// Internal watchdog handle
static IWDG_HandleTypeDef hiwdg;

// Tracks which task last petted the watchdog
static char lastTaskToPet[configMAX_TASK_NAME_LEN + 1] = "None";
WatchdogTask& WatchdogTask::Inst()
{
    static WatchdogTask instance;
    return instance;
}

void WatchdogTask::InitTask()
{

    xTaskCreate(TaskEntry, "Watchdog", 256, NULL, tskIDLE_PRIORITY + 3, NULL);
}

void WatchdogTask::TaskEntry(void* argument)
{

    #ifdef watchdogH7

    hiwdg.Instance = IWDG1;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
    hiwdg.Init.Reload = 1000;
    hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        SOAR_PRINT("IWDG Init Failed!\n");
        while (1);
    }

    SOAR_PRINT("Watchdog initialized\n");
   #else
    hiwdg.Instance = IWDG;

    hiwdg.Init.Prescaler = IWDG_PRESCALER_64;   
    hiwdg.Init.Reload    = 1000;              
    hiwdg.Init.Window    = IWDG_WINDOW_DISABLE;

    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        SOAR_PRINT("IWDG Init Failed!\n");
        while (1);
    }

    SOAR_PRINT("Watchdog initialized (G4)\n");

    #endif

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));

    }
}


void WatchdogTask::Pet()
{
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();

    if (handle != NULL)
    {
        const char* taskName = pcTaskGetName(handle);
        strncpy(lastTaskToPet, taskName, sizeof(lastTaskToPet));
        lastTaskToPet[sizeof(lastTaskToPet) - 1] = '\0';
    }
    SOAR_PRINT("WATCHDOG PET by: %s\n", pcTaskGetName(NULL));


    HAL_IWDG_Refresh(&hiwdg);
}

const char* WatchdogTask::GetLastTaskToPet()
{
    return lastTaskToPet;
}
