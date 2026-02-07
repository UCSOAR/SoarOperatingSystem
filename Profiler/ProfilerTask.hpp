/**
 ********************************************************************************
 * @file    Profiler.hpp
 * @author  Christy
 * @date    Jan 28, 2026
 * @brief
 ********************************************************************************
 */

#ifndef PROFILER_TASK_HPP_
#define PROFILER_TASK_HPP_
extern bool profileSystem;
/************************************
 * INCLUDES
 ************************************/
#include "Task.hpp"
#include "SystemDefines.hpp"
#include <vector>

/************************************
 * MACROS AND DEFINES
 ************************************/
extern bool profileSystem;  // profiling status bool

// struct to store task data
struct TaskProfile {
    char name[configMAX_TASK_NAME_LEN];
    char state[10];
    int priority;
    int stackRemaining;
    char cpuPercent[8];
};
/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * CLASS DEFINITIONS
 ************************************/
class ProfilerTask : public Task {
public:
    static ProfilerTask& Inst() {
        static ProfilerTask inst;
        return inst;
    }

    void InitTask();
    void ProfileSystem();   // public handle to run profiling

protected:
    // Main run code
    void Run(void* pvParams);

    // Static Task Interface, passes control to the instance Run();
    static void RunTask(void* pvParams) {
        ProfilerTask::Inst().Run(pvParams);
    } 

    // profiling methods
    void CollectTaskList(std::vector<TaskProfile>& Profiles);
    void CollectCPUStats(std::vector<TaskProfile>& Profiles);
    void DisplayTable(std::vector<TaskProfile>& Profiles);

private:
  ProfilerTask();                             // Private constructor
  ProfilerTask(const ProfilerTask&);             // Prevent copy-construction
  ProfilerTask& operator=(const ProfilerTask&);  // Prevent assignment

};
/************************************
 * FUNCTION DECLARATIONS
 ************************************/

#endif /* PROFILER_TASK_HPP_ */