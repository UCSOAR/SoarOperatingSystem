/*
 * CubeDefines.cpp
 *
 *  Created on: Oct 18, 2023
 *      Author: Chris
 */
#include <cstdarg>        // Support for va_start and va_end
#include <cstring>        // Support for strlen and strcpy

#include "Core/Inc/Mutex.hpp"
#include "Core/Inc/Command.hpp"
#include "Drivers/Inc/UARTDriver.hpp"
#include "CubeDefines.hpp"
#include "SystemDefines.hpp"
#include "CubeTask.hpp"

/* Global Variables ------------------------------------------------------------------*/
Mutex Global::vaListMutex;

/* System Functions ------------------------------------------------------------*/
/**
* @brief Variadic print function, sends a command packet to the queue
* @param str String to print with printf style formatting
* @param ... Additional arguments to print if assertion fails, in same format as printf
*/
void cube_print(const char* str, ...)
{
#ifndef DISABLE_DEBUG
    //Try to take the VA list mutex
    if (Global::vaListMutex.Lock(DEBUG_TAKE_MAX_TIME_MS)) {
        // If we have a message, and can use VA list, extract the string into a new buffer, and null terminate it
        uint8_t str_buffer[DEBUG_PRINT_MAX_SIZE] = {};
        va_list argument_list;
        va_start(argument_list, str);
        int16_t buflen = vsnprintf(reinterpret_cast<char*>(str_buffer), sizeof(str_buffer) - 1, str, argument_list);
        va_end(argument_list);
        if (buflen > 0) {
            str_buffer[buflen] = '\0';
        }

        // Release the VA List Mutex
        Global::vaListMutex.Unlock();

        //Generate a command
        Command cmd(DATA_COMMAND, (uint16_t)CUBE_TASK_COMMAND_SEND_DEBUG); // Set the UART channel to send data on

        //Copy data into the command
        cmd.CopyDataToCommand(str_buffer, buflen);

        //Send this packet off to the UART Task
        CubeTask::Inst().GetEventQueue()->Send(cmd, false);
    }
    else
    {
        // Print out that we could not acquire the VA list mutex
        SOAR_ASSERT(false, "Could not acquire VA_LIST mutex");
    }
#endif
}

/**
 * @brief Variadic assertion function, wraps assert for multi-platform support and debug builds
 * @param condition Assertion that this condition is true (!0)
 * @param file File that the assertion is in (__FILE__)
 * @param line Line number that the assertion is on (__LINE__)
 * @param str Optional message to print if assertion fails. Must be less than 192 characters AFTER formatting
 * @param ... Additional arguments to print if assertion fails, in same format as printf
 */
void cube_assert_debug(bool condition, const char* file, const uint16_t line, const char* str, ...) {
    // If assertion succeeds, do nothing
    if (condition) {
        return;
    }

#ifndef DISABLE_DEBUG

    bool printMessage = false;

    // NOTE: Be careful! If va_list funcs while RTOS is active ALL calls to any vsnprint functions MUST have a mutex lock/unlock
    // NOTE: https://nadler.com/embedded/newlibAndFreeRTOS.html

    // We have an assert fail, we try to take control of the Debug semaphore, and then suspend all other parts of the system
    if (Global::vaListMutex.Lock(ASSERT_TAKE_MAX_TIME_MS)) {
        // We have the mutex, we can now safely print the message
        printMessage = true;
    }

    vTaskSuspendAll();

    //If we have the vaListMutex, we can safely use vsnprintf
    if (printMessage) {
        // Print out the assertion header through the supported interface, we don't have a UART task running, so we directly use HAL
        uint8_t header_buf[ASSERT_BUFFER_MAX_SIZE] = {};
        int16_t res = snprintf(reinterpret_cast<char*>(header_buf), ASSERT_BUFFER_MAX_SIZE - 1, "\r\n\n-- ASSERTION FAILED --\r\nFile [%s] @ Line # [%d]\r\n", file, line);
        if (res < 0) {
            // If we failed to generate the header, just format the line number
            snprintf(reinterpret_cast<char*>(header_buf), ASSERT_BUFFER_MAX_SIZE - 1, "\r\n\n-- ASSERTION FAILED --\r\nFile [PATH_TOO_LONG] @ Line # [%d]\r\n", line);
        }

        // Output the header to the debug port
        DEFAULT_DEBUG_UART_DRIVER->Transmit(header_buf, strlen(reinterpret_cast<char*>(header_buf)));

        // If we have a message, and can use VA list, extract the string into a new buffer, and null terminate it
        if (printMessage && str != nullptr) {
            uint8_t str_buffer[ASSERT_BUFFER_MAX_SIZE] = {};
            va_list argument_list;
            va_start(argument_list, str);
            int16_t buflen = vsnprintf(reinterpret_cast<char*>(str_buffer), sizeof(str_buffer) - 1, str, argument_list);
            va_end(argument_list);
            if (buflen > 0) {
                str_buffer[buflen] = '\0';
                DEFAULT_DEBUG_UART_DRIVER->Transmit(str_buffer, buflen);
            }
        }
    }
    else {
        DEFAULT_DEBUG_UART_DRIVER->Transmit((uint8_t*)"-- ASSERTION FAILED --\r\nCould not acquire vaListMutex\r\n", 55);
    }

#endif

    HAL_NVIC_SystemReset();

    // We should not reach this code, but if we do, we should resume the scheduler
    xTaskResumeAll();
}
