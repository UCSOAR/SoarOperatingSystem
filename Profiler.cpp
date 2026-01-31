/**
 ********************************************************************************
 * @file    Profiler.cpp
 * @author  Christy
 * @date    Jan 28, 2026
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "Profiler.hpp"
#include "SystemDefines.hpp"
#include <string>
#include <cstring>
#include <vector>

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * VARIABLES
 ************************************/
std::vector<std::string> TaskNames;
/************************************
 * FUNCTION DECLARATIONS
 ************************************/

/************************************
 * FUNCTION DEFINITIONS
 ************************************/

void PrintTaskList() {
    // print list of tasks
    SOAR_PRINT("\n\nName        State   Priority  Stack   Num\r\n");
    SOAR_PRINT("**********************************************\r\n");

    char buffer[512];
    vTaskList(buffer);
    SOAR_PRINT("%s\n", buffer);

    // populate global vector of task names
    TaskNames.clear();

    // parse output and take first entry as task name
    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        char taskName[configMAX_TASK_NAME_LEN];
        if (sscanf(line, "%s", taskName) == 1) { // scan first entry splitting on whitespace in line
            TaskNames.push_back(std::string(taskName)); // append to global vector
        }
        line = strtok(NULL, "\n");
    }
    return;
}


void PrintCPUStats() {
    // print runtime stats using vTaskGetRunTimeStats
    SOAR_PRINT("\n\nCPU Runtime Stats\r\n");
    SOAR_PRINT("**********************************************\r\n");
    char buffer[512];
    vTaskGetRunTimeStats(buffer);
    SOAR_PRINT("%s\n", buffer);
}


void PrintHeap() {
    // print heap stats using xPortGetFreeHeapSize and xPortGetMinimumEverFreeHeapSize
    SOAR_PRINT("\n\nHeap Stats\r\n");
    SOAR_PRINT("**********************************************\r\n");
    SOAR_PRINT("Current System Free Heap: %d Bytes\r\n", xPortGetFreeHeapSize());
    SOAR_PRINT("Lowest Ever Free Heap: %d Bytes\r\n", xPortGetMinimumEverFreeHeapSize());
}


void PrintStack() {
    SOAR_PRINT("\n\nStack Remaining per Task\r\n");
    SOAR_PRINT("*************************\r\n");

    // iterate through global task name vector
    for (size_t i = 0; i < TaskNames.size(); ++i) {
        std::string& taskName = TaskNames[i];

        // get task handle for each task name
        TaskHandle_t handle = xTaskGetHandle(taskName.c_str());
        if (handle != NULL) {
            // print high watermark attribute for each task
            UBaseType_t highWater = uxTaskGetStackHighWaterMark(handle);
            SOAR_PRINT("%-12s : %lu words\r\n", taskName.c_str(), (unsigned long)highWater);
        }
    }
}


void ProfileSystem() {
    // call methods to print profile messages
    SOAR_PRINT("\r\n");
    PrintTaskList();
    PrintCPUStats();
    PrintHeap();
    PrintStack();
}