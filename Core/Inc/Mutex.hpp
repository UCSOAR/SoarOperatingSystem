/**
 ******************************************************************************
 * File Name          : Mutex.hpp
 * Description        : Mutex is an object wrapper for rtos mutexes.
 ******************************************************************************
*/
#ifndef CUBE_PLUSPLUS_INCLUDE_CORE_MUTEX_H
#define CUBE_PLUSPLUS_INCLUDE_CORE_MUTEX_H

/* Includes ------------------------------------------------------------------*/
#include "CubeRTOS.hpp"    // FreeRTOS kernel API (CMSIS_V1/V2 agnostic)

/* Macros --------------------------------------------------------------------*/


/* Class -----------------------------------------------------------------*/

/**
 * @brief Mutex class is a wrapper for rtos mutexes.
 */
class Mutex
{
public:
    // Constructors / Destructor
    Mutex();
    ~Mutex();

    // Public functions
    bool Lock(uint32_t timeout_ms = portMAX_DELAY);
    bool Unlock();

    bool LockFromISR();
    bool UnlockFromISR();

private:
    SemaphoreHandle_t rtSemaphoreHandle;

};


#endif /* CUBE_PLUSPLUS_INCLUDE_CORE_MUTEX_H */
