# Introduction
A board independent hardware watchdog task in SOAROS. If a task stops petting the watchdog, the board will reset automatically.
Supports STM32H7 (IWDG1) and STM32G4 (IWDG)

# Setup Instructions

## 1. Add Include Path in CubeIDE
Right click project → Properties → C/C++ Build → Settings → 
MCU G++ Compiler → Include paths → add:
../SoarOS/Components/Watchdog

## 2. make sure the Watchdog is initialize in main_system.cpp
#include "WatchdogTask.hpp"

 Inside run_main() put:

WatchdogTask::Inst().InitTask();

## 3. Pet the Watchdog in Each Critical Task
Add to the beginning of the while(1) loop of every critical task:

WatchdogTask::Inst().Pet();



# IOC Setup for H7VIT6
- Open your .ioc file in STM32CubeIDE
- Go to Pinout & Configuration
- Under System Core find IWDG1
- Enable it by setting Activated to checked
- Save and regenerate code

# IOC Setup for G491ME
- Open your .ioc file in STM32CubeIDE
- Go to Pinout & Configuration
- Under System Core find IWDG
- Enable it by setting Activated to checked
- Save and regenerate code

