<div align="center">
<img alt="Cube++" src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/025c59ea-36be-4131-9b09-06c37ff81b52" width="300"/>
</div>

<a href="https://en.cppreference.com/w/" rel="CppReference">![Foo](https://user-images.githubusercontent.com/78698227/185344746-ace6a502-a9a3-43d6-9379-daad6f4ffcd3.svg)</a>
<a href="https://www.st.com/" rel="STMicro">![STMicro](https://user-images.githubusercontent.com/78698227/185344511-0296b5ed-15a3-4013-a98a-6dcd38222382.svg)</a>
<a href="https://www.freertos.org/features.html" rel="FreeRTOS">![FreeRTOS](https://img.shields.io/badge/FreeRTOS-%23green.svg?style=for-the-badge)</a>
<a href="https://github.com/cjchanx/CubePlusPlus.git" rel="GitHub">![Git](https://img.shields.io/badge/git-%23F05033.svg?style=for-the-badge&logo=git&logoColor=white)</a>

![](https://img.shields.io/github/repo-size/cjchanx/CubePlusPlus?label=Size)
![](https://img.shields.io/github/commit-activity/m/cjchanx/CubePlusPlus)
![](https://img.shields.io/github/contributors/cjchanx/CubePlusPlus)

# Introduction
Cube++ simplifies the process of creating a C/C++ codebase for an STM32 microcontroller in STM32CubeIDE while fully supporting CubeIDE's native code generation. This project is intended to be a submodule in a CubeIDE project to give access to some useful C++ wrappers and a method of routing control from the C main.c file to the C++ codebase located in one dedicated folder to support cleaner code compared to mixing it with code generation, and preventing the need for constantly renaming `main.c` to `main.cpp` every time code generation happens. Please refer to the [Setup Instructions](#Setup-Instructions) below for a guide on how to setup the project with Cube++

Cube++ is based on the core code of the [Student Organization for Aerospace Research's Avionics Software](https://github.com/StudentOrganisationForAerospaceResearch/AvionicsSoftware/tree/d3b55eb04511bf8b2d5ea3fb2d51fbd1329d9e81). As the codebase was originally intended for use in a specific way there are still updates to be made to improve compatability and capabilities.

# Setup Instructions 
1. [CubeIDE Project Setup](#CubeIDE-Project-Setup)
2. [IOC Setup](#IOC-Setup)
3. [Cube++ Submodule Setup](#Cube-Setup)
4. [Codebase Setup](#Codebase-Setup)

## CubeIDE Project Setup
- Create a new project for your specific chip and select C++ when it asks you for Language

## IOC Setup
Open the `.ioc` file by double clicking on it inside the project

### Middleware Setup

- Under `Middleware and Software Packs` enable FreeRTOS using `CMSIS_V1` (unless you are using a multi-core CPU in which case `CMSIS_V2` may be preferred)
- Change the following values in **config parameters**
    - FreeRTOS Heap Size to fairly large (eg. 64KB on a 90KB RAM chip)
    - MINIMAL_STACK_SIZE to at least 192 Words
    - Under Software Timer Defines "USE_TIMERS" should be enabled
<img src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/ba5d8d45-e6c5-4aa3-8369-d16eca3f6293" width="450">
<img src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/387edd29-ffa4-4fce-9561-434416cd2f09" width="275">

- Under `Middleware and Software Packs` > FreeRTOS > Advanced Settings
    - Enable USE_NEWLIB_REENTRANT
<img src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/ba866dcf-580e-4cec-9757-6abbe57bb5f2" width="325">



- Under Computing > CRC 
    - Enable Activated
<img src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/ae1745da-5d21-4765-86eb-5f6c0536f1cf" width="400">

- Under `System Core` > SYS > Timebase Source
    - Change the timebase source to a timer instead of SysTick (CubeIDE will recommend you do this upon code generation)
<img src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/8444fd51-31ce-47e1-a7a0-d3c35d4e351a" width="450">


### IOC Peripheral Setup
#### For UART Debug Line 
**(if not defining `DEBUG_DISABLED` as mentioned in the [Codebase Setup](#Codebase-Setup) section below)**

- Inside Pinout and Configuration > Connectivity
    - Select a UART/USART line for debug (if you choose to enable it) and set to Asynchronous mode with hardware flow control off
<img src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/e03452b3-1d6f-4883-8e97-1fdd8ad94036" width="450">

- At the top select Project Manager tab, go to the Advanced Settings and select the UART line for debug under Driver Selector
    - Change this UART line driver from `HAL` to `LL`
<img src="https://github.com/cjchanx/CubePlusPlus/assets/78698227/830aa70e-299c-44ae-9bdd-f139266ce833" width="450">



## Cube++ Setup
Add the Cube++ repository into the root folder as a submodule using one of these commands

SSH
```
git submodule add git@github.com:UCSOAR/SoarOperatingSystem.git SoarOS
```
HTTPS
```
git submodule add https://github.com/UCSOAR/SoarOperatingSystem.git SoarOS
```

Please add the following to include paths:
(right click on the folder and select `Add/Remove Include Path...`)
- Cube++/Core
- Cube++/Drivers/Inc
- Cube++/Libraries/embedded-template-library/include

Please ensure the following folders are not in the exclude from build option:
(right click on the folder, select properties, select configuration 'all configuration' and untick "select `Exclude Resource from Build`")
- (none for this version)

### Codebase Setup
An example project utilizing Cube++ with basic SOAR_PRINT support, in addition to a Debug receive task for parsing input data on the debug line can be found at https://github.com/cjchanx/cubeplusplus-examples/tree/main/Basic_Debug

- It is recommended to setup a new folder called `Components` or `Modules` in the root where all the code goes.
- There are a few files that you should have in `Components`/`Modules`:
	- SystemDefines.hpp : An example can be found [here](https://github.com/cjchanx/cubeplusplus-examples/blob/main/Basic_Debug/Components/SystemDefines.hpp)
	- main_avionics.cpp   : This can be named anything you want, but should contain the run_main() function that is the entry point for your codebase, example [here](https://github.com/cjchanx/cubeplusplus-examples/blob/main/Basic_Debug/Components/main_avionics.cpp)
	- main_avionics.hpp   : Header file for main_avionics.cpp, example [here](https://github.com/cjchanx/cubeplusplus-examples/blob/main/Basic_Debug/Components/main_avionics.hpp)
	- SysCore/Inc/RunInterface.hpp : Header file for the run interface which allows C code to call into the C++ codebase without errors, example [here](https://github.com/cjchanx/cubeplusplus-examples/blob/main/Basic_Debug/Components/Core/Inc/RunInterface.hpp)
	- SysCore/RunInterface.cpp : Code file for the run interface, example [here](https://github.com/cjchanx/cubeplusplus-examples/blob/main/Basic_Debug/Components/Core/RunInterface.cpp)
- Setup Debug UART
	- Setup one UART line as the UART debug line by following theinstructions uder Cube++/Drivers/UARTDriver_README.md, this should correspond to UART Driver linked to the 
	`constexpr UARTDriver* const DEFAULT_DEBUG_UART_DRIVER = UART::Debug;` line that is required to be in the SystemDefines.hpp file
	- In the case that you do not want a UART line dedicated to debug, then set DEFAULT_DEBUG_UART_DRIVER to nullptr, and
	define the macro DISABLE_DEBUG using `#define DISABLE_DEBUG` inside SystemDefines.hpp
- Setup main.c
	- Under Private includes in the first USER CODE BEGIN section, add the run interface as an include `RunInterface.hpp`
	- Inside the int main(void) function definition, add the following inside `USER CODE BEGIN 2`:
```
  run_interface();
  #if 0
```
	- Then inside `USER CODE BEGIN 3` after the `}` you want to put an `#endif`
	- This ensures that the only tasks that will be started up are the ones you add to your run_main() function
 

# Solving Issues
## Components/Cube++ excluded from build
- CubeIDE may exclude newly added folders from the build, to include it right click on the folder, go into Properties > C/C++ Build (on the left bar) > Select Configuration: `[All Configurations]` in the dropdown box, and then untick "Exclude Resource from Build"

## Errors building Embedded Template Library
- The included etl_profile is set for C++17 and up, change the C/C++ build settings to use the GNU++17 language standard
- Right click the project in the project explorer > Properties > Expand C/C++ Build > Settings > General under MCU G++ Compiler > Language Standard
- Change to GNU++17 (ISO C++17 + gnu extensions)(-std=gnu++17) or up
<img src="https://github.com/user-attachments/assets/7a787c0f-f7ad-4fcc-90fe-cd928f29ceaa" width="450">

## Warnings Voltatile and Register when using C++17
- Recommended to add the following build flags
```
-Wno-register
-Wno-volatile
```
- Right click the project in the project explorer > Properties > Expand C/C++ Build > Settings > Miscellaneous under MCU G++ Compiler
<img src="https://github.com/user-attachments/assets/2a83056c-2803-40c3-85aa-41dda7d9c48e" width="450">


