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
#include "ProfilerTask.hpp"
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
bool profileSystem = false;

/************************************
 * FUNCTION DECLARATIONS
 ************************************/

/************************************
 * FUNCTION DEFINITIONS
 ************************************/
/**
 * @brief Constructor, sets all member variables
 */
ProfilerTask::ProfilerTask()
    : Task(TASK_PROFILER_QUEUE_DEPTH_OBJS) {}

/**
 * @brief Init task for RTOS
 */
void ProfilerTask::InitTask() {
  // Make sure the task is not already initialized
  SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize Profiler task twice");

  // Start the task
  BaseType_t rtValue = xTaskCreate(
      (TaskFunction_t)ProfilerTask::RunTask, (const char*)"ProfilerTask",
      (uint16_t)TASK_PROFILER_STACK_DEPTH_WORDS, (void*)this,
      (UBaseType_t)TASK_PROFILER_PRIORITY, (TaskHandle_t*)&rtTaskHandle);

  // Ensure creation succeded
  SOAR_ASSERT(rtValue == pdPASS, "ProfilerTask::InitTask - xTaskCreate() failed");
}


/**
 *    @brief Runcode for the ProfilerTask
 */
void ProfilerTask::Run(void* pvParams) {
  while (1) {
        // profile system while profileSystem is true
        if (profileSystem) {
            ProfileSystem();
        }
        vTaskDelay(pdMS_TO_TICKS(2000));    // delay interval
    }
}


void ProfilerTask::CollectTaskList(std::vector<TaskProfile>& Profiles) {
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

        // read 5 attributes succesfully
        if (sscanf(line, "%15[^ ] %c %d %d %d", name, &state, &priority, &stack, &num) == 5) {
            TaskProfile profile;

            // if name, state, priority, stack, and num successfully read populate profile object
            strncpy(profile.name, name, sizeof(profile.name));

            // convert state to human readable format
            std::string readableState;
            switch (state) {
                case 'R': readableState = "Ready"; 
                break;
                case 'B': readableState = "Blocked"; 
                break;
                case 'S': readableState = "Suspended"; 
                break;
                case 'X': readableState = "Executing"; 
                break;
                default:  readableState = "Unknown"; 
                break;
            }
            strncpy(profile.state, readableState.c_str(), sizeof(profile.state));
            profile.priority = priority;
            profile.stackRemaining = stack;
            strcpy(profile.cpuPercent, "?");

            // append task profile to global vector
            Profiles.push_back(profile);
        }

        line = strtok(NULL, "\n");
    }
}


void ProfilerTask::CollectCPUStats(std::vector<TaskProfile>& Profiles) {
    // place vTaskGetRunTimeStats output into buffer
    char buffer[512];
    vTaskGetRunTimeStats(buffer);

    // parse runtime stats
    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        char name[32];
        char percent[8];

        // read 2 attributes succesfully
        if (sscanf(line, "%15s %*s %3s", name, percent) == 2) { // ignore abs time
            // if name and time percentage sucessfully read, find task in global vector
            for (auto& p : Profiles) {
                if (strcmp(p.name, name) == 0) {
                    // initialize task percentage
                    strncpy(p.cpuPercent, percent, sizeof(p.cpuPercent));
                }
            }
        }

        line = strtok(NULL, "\n");
    }
}


void ProfilerTask::DisplayTable(std::vector<TaskProfile>& Profiles) {
    // print profile message for tasks
    SOAR_PRINT("Task Name, Task State, Task Priority, Stack High Water Mark (words), Task CPU Usage\r\n\n");

    for (auto& p : Profiles) {
        SOAR_PRINT("%s, %s, %d, %d, %s\r\n", p.name, p.state, p.priority, p.stackRemaining, p.cpuPercent);
    }
}


void ProfilerTask::ProfileSystem() {
    // clear terminal, previous task profiles
    SOAR_PRINT("\033[2J\033[H");

    // store task structs in vector
    std::vector<TaskProfile> Profiles; 

    // collect profile stats, display profile stat table
    CollectTaskList(Profiles);
    CollectCPUStats(Profiles);
    DisplayTable(Profiles);

    // display heap stats
    SOAR_PRINT("\r\nFree Heap (bytes), Min Ever Free Heap (bytes)");
    SOAR_PRINT("\r\n%lu, %lu", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
}
