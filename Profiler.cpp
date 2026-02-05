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
// std::vector<std::string> TaskNames;

struct TaskProfile {
    std::string name;
    char state;
    int priority;
    int stackRemaining;
    std::string cpuPercent;
};

std::vector<TaskProfile> Profiles;
// /************************************
//  * FUNCTION DECLARATIONS
//  ************************************/

// /************************************
//  * FUNCTION DEFINITIONS
//  ************************************/

// void PrintTaskList() {
//     // print list of tasks
//     SOAR_PRINT("\n\nName        State   Priority  Stack   Num\r\n");
//     SOAR_PRINT("**********************************************\r\n");

//     char buffer[512];
//     vTaskList(buffer);
//     SOAR_PRINT("%s\n", buffer);

//     // populate global vector of task names
//     TaskNames.clear();

//     // parse output and take first entry as task name
//     char* line = strtok(buffer, "\n");
//     while (line != NULL) {
//         char taskName[configMAX_TASK_NAME_LEN];
//         if (sscanf(line, "%s", taskName) == 1) { // scan first entry splitting on whitespace in line
//             TaskNames.push_back(std::string(taskName)); // append to global vector
//         }
//         line = strtok(NULL, "\n");
//     }
//     return;
// }


// void PrintCPUStats() {
//     // print runtime stats using vTaskGetRunTimeStats
//     SOAR_PRINT("\n\nCPU Runtime Stats\r\n");
//     SOAR_PRINT("**********************************************\r\n");
//     char buffer[512];
//     vTaskGetRunTimeStats(buffer);
//     SOAR_PRINT("%s\n", buffer);
// }


// void PrintHeap() {
//     // print heap stats using xPortGetFreeHeapSize and xPortGetMinimumEverFreeHeapSize
//     SOAR_PRINT("\n\nHeap Stats\r\n");
//     SOAR_PRINT("**********************************************\r\n");
//     SOAR_PRINT("Current System Free Heap: %d Bytes\r\n", xPortGetFreeHeapSize());
//     SOAR_PRINT("Lowest Ever Free Heap: %d Bytes\r\n", xPortGetMinimumEverFreeHeapSize());
// }


// void PrintStack() {
//     SOAR_PRINT("\n\nStack Remaining per Task\r\n");
//     SOAR_PRINT("*************************\r\n");

//     // iterate through global task name vector
//     for (size_t i = 0; i < TaskNames.size(); ++i) {
//         std::string& taskName = TaskNames[i];

//         // get task handle for each task name
//         TaskHandle_t handle = xTaskGetHandle(taskName.c_str());
//         if (handle != NULL) {
//             // print high watermark attribute for each task
//             UBaseType_t highWater = uxTaskGetStackHighWaterMark(handle);
//             SOAR_PRINT("%-12s : %lu words\r\n", taskName.c_str(), (unsigned long)highWater);
//         }
//     }
// }


// void ProfileSystem() {
//     // call methods to print profile messages
//     SOAR_PRINT("\r\n");
//     PrintTaskList();
//     PrintCPUStats();
//     PrintHeap();
//     PrintStack();
// }

























void CollectTaskList() {
    // place vTaskList output into buffer
    char buffer[512];
    vTaskList(buffer);

    // parse tasks from line
    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        char name[configMAX_TASK_NAME_LEN];
        char state;
        int priority;
        int stack;
        int num;

        if (sscanf(line, "%15[^ ] %c %d %d %d", name, &state, &priority, &stack, &num) == 5) {
            TaskProfile profile;

            // if name, state, priority, stack, and num successfully read populate profile object
            profile.name = name;
            profile.state = state;
            profile.priority = priority;
            profile.stackRemaining = stack;
            profile.cpuPercent = "?";

            // append task profile to global vector
            Profiles.push_back(profile);
        }

        line = strtok(NULL, "\n");
    }
}


void CollectCPUStats() {
    // place vTaskGetRunTimeStats output into buffer
    char buffer[512];
    vTaskGetRunTimeStats(buffer);

    // parse runtime stats
    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        char name[32];
        char percent[8];

        if (sscanf(line, "%15s %*s %3s", name, percent) == 2) { // ignore abs time
            // if name and time percentage sucessfully read, find task in global vector
            for (auto& p : Profiles) {
                if (p.name == name) {
                    // initialize task percentage
                    p.cpuPercent = percent;
                }
            }
        }

        line = strtok(NULL, "\n");
    }
}



void DisplayTable() {
    // header
    SOAR_PRINT("\r\nSystem Profile\r\n");
    SOAR_PRINT("================================================================================\r\n");

    // print task names
    SOAR_PRINT("%-12s", "Metric\\Task");
    for (auto& p : Profiles)
        SOAR_PRINT("| %-10s ", p.name.c_str());
    SOAR_PRINT("\r\n");
    SOAR_PRINT("--------------------------------------------------------------------------------\r\n");

    // print task state
    SOAR_PRINT("%-12s", "State");
    for (auto& p : Profiles)
        SOAR_PRINT("| %-10c ", p.state);
    SOAR_PRINT("\r\n");

    // print task priority
    SOAR_PRINT("%-12s", "Priority");
    for (auto& p : Profiles)
        SOAR_PRINT("| %-10d ", p.priority);
    SOAR_PRINT("\r\n");

    // print task high water mark (stack remaining/closer to 0 means stack is running out)
    SOAR_PRINT("%-12s", "Stack Rem");
    for (auto& p : Profiles)
        SOAR_PRINT("| %-10d ", p.stackRemaining);
    SOAR_PRINT("\r\n");

    // print task cpu time percent use
    SOAR_PRINT("%-12s", "CPU %");
    for (auto& p : Profiles)
        SOAR_PRINT("| %-10s ", p.cpuPercent.c_str());
    SOAR_PRINT("\r\n");
    SOAR_PRINT("================================================================================\r\n");
}


void ProfileSystem() {
    // clear terminal, previous task profiles
    SOAR_PRINT("\033[2J\033[H");
    Profiles.clear();

    // collect profile stats, display profile stat table
    CollectTaskList();
    CollectCPUStats();
    DisplayTable();

    // display heap stats
    SOAR_PRINT("\r\nFree Heap: %lu bytes | Min Ever Free Heap: %lu bytes\r\n",
               xPortGetFreeHeapSize(),
               xPortGetMinimumEverFreeHeapSize());
}
