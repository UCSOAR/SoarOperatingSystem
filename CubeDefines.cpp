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
#include "mx66xx_qspi.hpp"

/* Global Variables ------------------------------------------------------------------*/
Mutex Global::vaListMutex;

namespace
{
constexpr uint8_t MX66XX_QSPI_SR1_BP_MASK = 0x3CU;
constexpr uint8_t MX66XX_QSPI_SR1_BP_TOP_1_OVER_64 = 0x04U;
constexpr uint32_t FLASH_LOG_SECTOR = (FS_TOTAL_SIZE / FS_SECTOR_SIZE) - 2U; // Second-last 4KB sector

Mutex gFlashLogMutex;
uint32_t gFlashLogOffset = 0U;
bool gFlashLogInitialized = false;

bool FlashLogEnsureInitialized()
{
    if (gFlashLogInitialized)
    {
        return true;
    }

    if (mx66xx_qspi.SectorSize == 0U || mx66xx_qspi.SectorCount == 0U)
    {
        if (!MX66xxQSPI_Init())
        {
            return false;
        }
    }

    if (FLASH_LOG_SECTOR >= mx66xx_qspi.SectorCount)
    {
        return false;
    }

    uint8_t sectorBuf[FS_SECTOR_SIZE] = {};
    MX66xxQSPI_ReadSector(sectorBuf, FLASH_LOG_SECTOR, 0U, FS_SECTOR_SIZE);

    gFlashLogOffset = FS_SECTOR_SIZE;
    for (uint32_t i = 0U; i < FS_SECTOR_SIZE; ++i)
    {
        if (sectorBuf[i] == 0xFFU)
        {
            gFlashLogOffset = i;
            break;
        }
    }

    gFlashLogInitialized = true;
    return true;
}

bool FlashLogAppendBytes(const uint8_t* data, uint32_t len)
{
    if (data == nullptr || len == 0U)
    {
        return true;
    }

    if (!FlashLogEnsureInitialized())
    {
        return false;
    }

    if (gFlashLogOffset >= FS_SECTOR_SIZE)
    {
        return false;
    }

    uint32_t writable = FS_SECTOR_SIZE - gFlashLogOffset;
    if (writable == 0U)
    {
        return false;
    }

    uint32_t writeLen = (len < writable) ? len : writable;
    const uint32_t writeOffset = gFlashLogOffset;
    MX66xxQSPI_WriteSector(const_cast<uint8_t*>(data), FLASH_LOG_SECTOR, writeOffset, writeLen);

    uint8_t verifyBuf[64] = {};
    uint32_t verified = 0U;
    bool writeVerified = true;
    while (verified < writeLen)
    {
        uint32_t chunk = writeLen - verified;
        if (chunk > sizeof(verifyBuf))
        {
            chunk = sizeof(verifyBuf);
        }

        MX66xxQSPI_ReadSector(verifyBuf, FLASH_LOG_SECTOR, writeOffset + verified, chunk);
        if (memcmp(&data[verified], verifyBuf, chunk) != 0)
        {
            writeVerified = false;
            break;
        }

        verified += chunk;
    }

    if (writeVerified)
    {
        gFlashLogOffset += writeLen;
    }

    return writeVerified && (writeLen == len);
}

void CubePrintFromBuffer(const uint8_t* strBuffer, uint16_t txLen)
{
    Command cmd(DATA_COMMAND, (uint16_t)CUBE_TASK_COMMAND_SEND_DEBUG);
    cmd.CopyDataToCommand(strBuffer, txLen);
    CubeTask::Inst().GetEventQueue()->Send(cmd, false);
}
}

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
        uint16_t txLen = 0;
        if (buflen > 0) {
            // vsnprintf returns the number of chars that *would* have been written,
            // so clamp to the local buffer capacity before indexing/copying.
            txLen = static_cast<uint16_t>(buflen);
            if (txLen >= sizeof(str_buffer)) {
                txLen = sizeof(str_buffer) - 1;
            }
            str_buffer[txLen] = '\0';
        }

        // Release the VA List Mutex
        Global::vaListMutex.Unlock();

        //Send this packet off to the UART Task
        CubePrintFromBuffer(str_buffer, txLen);
    }
    else
    {
        // Print out that we could not acquire the VA list mutex
        SOAR_ASSERT(false, "Could not acquire VA_LIST mutex");
    }
#endif
}

void cube_print_flash(const char* str, ...)
{
#ifndef DISABLE_DEBUG
    if (Global::vaListMutex.Lock(DEBUG_TAKE_MAX_TIME_MS)) {
        uint8_t str_buffer[DEBUG_PRINT_MAX_SIZE] = {};
        va_list argument_list;
        va_start(argument_list, str);
        int16_t buflen = vsnprintf(reinterpret_cast<char*>(str_buffer), sizeof(str_buffer) - 1, str, argument_list);
        va_end(argument_list);

        uint16_t txLen = 0;
        if (buflen > 0) {
            txLen = static_cast<uint16_t>(buflen);
            if (txLen >= sizeof(str_buffer)) {
                txLen = sizeof(str_buffer) - 1;
            }
            str_buffer[txLen] = '\0';
        }

        Global::vaListMutex.Unlock();

        CubePrintFromBuffer(str_buffer, txLen);

        if (gFlashLogMutex.Lock(DEBUG_TAKE_MAX_TIME_MS))
        {
            (void)FlashLogAppendBytes(str_buffer, txLen);
            gFlashLogMutex.Unlock();
        }
    }
    else
    {
        SOAR_ASSERT(false, "Could not acquire VA_LIST mutex");
    }
#endif
}

bool cube_flash_log_lock_sector()
{
    if (!gFlashLogMutex.Lock(DEBUG_TAKE_MAX_TIME_MS))
    {
        return false;
    }

    bool ok = false;
    if (FlashLogEnsureInitialized())
    {
        const uint8_t currentSr1 = MX66xxQSPI_ReadStatusRegister();
        const uint8_t lockedSr1 = (uint8_t)((currentSr1 & ~MX66XX_QSPI_SR1_BP_MASK) | MX66XX_QSPI_SR1_BP_TOP_1_OVER_64);
        MX66xxQSPI_WriteStatusRegister(lockedSr1);
        ok = (MX66xxQSPI_ReadStatusRegister() & MX66XX_QSPI_SR1_BP_MASK) != 0U;
    }

    gFlashLogMutex.Unlock();
    return ok;
}

bool cube_flash_log_test_locked_write()
{
    if (!gFlashLogMutex.Lock(DEBUG_TAKE_MAX_TIME_MS))
    {
        return false;
    }

    bool blocked = false;
    if (FlashLogEnsureInitialized())
    {
        static const char kLockProbe[] = "SOAR_FLASH_LOCK_PROBE";
        constexpr uint32_t kProbeLen = (uint32_t)(sizeof(kLockProbe) - 1U);

        if (gFlashLogOffset + kProbeLen <= FS_SECTOR_SIZE)
        {
            const uint32_t probeOffset = gFlashLogOffset;
            uint8_t before[kProbeLen] = {};
            MX66xxQSPI_ReadSector(before, FLASH_LOG_SECTOR, probeOffset, kProbeLen);

            (void)FlashLogAppendBytes(reinterpret_cast<const uint8_t*>(kLockProbe), kProbeLen);

            uint8_t after[kProbeLen] = {};
            MX66xxQSPI_ReadSector(after, FLASH_LOG_SECTOR, probeOffset, kProbeLen);

            blocked = (memcmp(before, after, kProbeLen) == 0);
        }
    }

    gFlashLogMutex.Unlock();
    return blocked;
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

    // we should NOT reset on a flight!
    HAL_NVIC_SystemReset();

    // We should not reach this code, but if we do, we should resume the scheduler
    xTaskResumeAll();
}
