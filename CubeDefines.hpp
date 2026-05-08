/**
  ******************************************************************************
  * @file           : CubeDefines.hpp
  * @brief          : Cube++ Macros and wrappers
  ******************************************************************************
  *
  * Contains system wide macros, defines, and wrappers
  *
  ******************************************************************************
  */
#ifndef CUBE_PLUSPLUS_CUBE_DEFINES_HPP_
#define CUBE_PLUSPLUS_CUBE_DEFINES_HPP_

/* System Wide Includes ------------------------------------------------------------------*/
#include "CubeUtils.hpp"  // Utility functions
#include <cstdint>        // For uint32_t, etc.
#include <cstdio>        // Standard c printf, vsnprintf, etc.
#include "cmsis_os.h"    // CMSIS RTOS definitions
#include "Mutex.hpp"

/* Global Functions ------------------------------------------------------------------*/
void cube_print(const char* format, ...);
void cube_assert_debug(bool condition, const char* file, uint16_t line, const char* str = nullptr, ...);

/* Global Variable Interfaces ------------------------------------------------------------------*/
/* All must be extern from CubeDefines.cpp -------------------------------------------------*/
namespace Global
{
    extern Mutex vaListMutex; // Note: This mutex MUST be used for any function parsing variadic arguments
}

/* System Defines ------------------------------------------------------------------*/
/* - Each define / constexpr must have a comment explaining what it is used for     */
/* - Each define / constexpr must be all-caps. Prefer constexpr unless it's a string, or a calculation (eg. mathematical expression being more readable) */
// RTOS
constexpr uint8_t DEFAULT_QUEUE_SIZE = 10;                    // Default size of the queue
constexpr uint16_t MAX_NUMBER_OF_COMMAND_ALLOCATIONS = 100;    // Let's assume ~128B per allocation, 100 x 128B = 12800B = 12.8KB

// DEBUG
constexpr uint16_t DEBUG_TAKE_MAX_TIME_MS = 500;        // Max time in ms to take the debug semaphore
constexpr uint16_t DEBUG_SEND_MAX_TIME_MS = 500;        // Max time the assert fail is allowed to wait to send header and message to HAL
constexpr uint16_t DEBUG_PRINT_MAX_SIZE = 192;            // Max size in bytes of message print buffers

// ASSERT
constexpr uint16_t ASSERT_BUFFER_MAX_SIZE = 160;        // Max size in bytes of assert buffers (assume x2 as we have two message segments)
constexpr uint16_t ASSERT_SEND_MAX_TIME_MS = 250;        // Max time the assert fail is allowed to wait to send header and message to HAL (will take up to 2x this since it sends 2 segments)
constexpr uint16_t ASSERT_TAKE_MAX_TIME_MS = 500;        // Max time in ms to take the assert semaphore

/* System Functions ------------------------------------------------------------------*/
//- Any system functions with an implementation here should be inline, and inline for a good reason (performance)
//- Otherwise the function may have a better place in main_avionics.cpp

// Assert macro, use this for checking all possible program errors eg. malloc success etc. supports a custom message in printf format
// This is our version of the stm32f4xx_hal_conf.h 'assert_param' macro with support for optional messages
// Example Usage: SOAR_ASSERT(ptr != 0, "Pointer on loop index %d is null!", index);
#define SOAR_ASSERT(expr, ...) ((expr) ? (void)0U : cube_assert_debug(false, (const char *)__FILE__, __LINE__, ##__VA_ARGS__))

// SOAR_PRINT macro, acts as an interface to the print function which sends a packet to the UART Task to print data
#define SOAR_PRINT(str, ...) (cube_print(str, ##__VA_ARGS__))

/**
 * @brief Malloc inline function, wraps malloc for multi-platform support, asserts successful allocation
 * @param size Size of data to malloc in bytes
 * @return Returns the pointer to the allocated data
*/
inline uint8_t* cube_malloc(uint32_t size) {
#ifdef COMPUTER_ENVIRONMENT
    uint8_t* ret = (uint8_t*)malloc(size);
#else
    uint8_t* ret = (uint8_t*)pvPortMalloc(size);
#endif
    SOAR_ASSERT(ret, "cube_malloc failed");
    return ret;
}

/**
 * @brief Free inline function, wraps free for multi-platform support
 * @param ptr Pointer to the data to free
 */
inline void cube_free(void* ptr) {
#ifdef COMPUTER_ENVIRONMENT
    free(ptr);
#else
    vPortFree(ptr);
#endif
}

/* Other ------------------------------------------------------------------*/
// Override the new and delete operator to ensure heap4 is used for dynamic memory allocation
inline void* operator new(size_t size) { return cube_malloc(size); }
inline void operator delete(void* ptr) { cube_free(ptr); }


#endif // CUBE_PLUSPLUS_CUBE_DEFINES_HPP_
