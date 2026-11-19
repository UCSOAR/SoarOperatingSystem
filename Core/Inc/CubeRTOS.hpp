/**
 ******************************************************************************
 * File Name          : CubeRTOS.hpp
 * Description        : Single include point for the FreeRTOS kernel API.
 *
 *    Cube++ uses the native FreeRTOS API directly and does not depend on the
 *    CMSIS-RTOS wrapper, so it works with FreeRTOS generated using either
 *    CMSIS_V1 or CMSIS_V2 in CubeMX / CubeIDE.
 *
 *    Include this instead of "cmsis_os.h". Note the CMSIS_V2 cmsis_os.h only
 *    pulls in FreeRTOS.h and task.h, so the queue / semaphore / timer headers
 *    must be included explicitly.
 ******************************************************************************
*/
#ifndef CUBE_PLUSPLUS_INCLUDE_CORE_CUBE_RTOS_HPP_
#define CUBE_PLUSPLUS_INCLUDE_CORE_CUBE_RTOS_HPP_

#include <cstdint>

#include "FreeRTOS.h"    // Must be included before any other FreeRTOS header
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#endif /* CUBE_PLUSPLUS_INCLUDE_CORE_CUBE_RTOS_HPP_ */
