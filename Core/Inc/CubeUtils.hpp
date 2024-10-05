/**
 ******************************************************************************
 * File Name          : Utils.hpp
 * Description        : Utility functions, accessible system wide.
 *                Includes functions for converting between data types
 *                Use very specific naming, or a namespace to avoid conflicts
 *                Keep namespace Utils to avoid conflicts with other libraries
 ******************************************************************************
*/
#ifndef CUBE_PLUSPLUS_INCLUDE_UTILS_HPP_
#define CUBE_PLUSPLUS_INCLUDE_UTILS_HPP_
#include "cmsis_os.h"    // CMSIS RTOS definitions

// Programmer Macros
constexpr uint16_t ERRVAL = 0xDEAD;    // 16-bit Error value for debugging
constexpr uint32_t ERRVAL32 = 0xDEADBEEF; // 32-bit Error value for debugging

// Math Macros and Conversions
constexpr double MATH_PI = 3.14159265358979323846;
#define DEG_TO_RAD(degrees) ((degrees) * 0.01745329251994329576923690768489f)    // Degrees to radians (PI/180)
#define RAD_TO_DEG(radians) ((radians) * 57.295779513082320876798154814105f)    // Radians to degrees (180/PI)
#define MILLIG_TO_MPS2(millig) ((millig) * 9.80665f)    // Milli-g to m/s^2
#define MILLIDPS_TO_RADPS(millidps) ((millidps) * 0.00017453292519943295769236907684886f)    // Milli-degrees per second to radians per second (PI/180/1000)
#define LBS_TO_GRAMS(lbs) ((lbs) * 453.59237f)    // Pounds to grams
#define MPS2_TO_MILLIG(mps2) ((mps2) / 9.80665f)    // m/s^2 to Milli-g
#define RADPS_TO_MILLIDPS(radps) ((radps) / 0.00017453292519943295769236907684886f)    // radians per second to Milli-degrees per second (PI/180/1000)
#define GRAMS_TO_LBS(grams) ((grams) / 453.59237f)    // grams to pounds
#define GET_COBS_MAX_LEN(len) (((len) + ((len) / 254) + 1) + 1)    // Get the max length of a COBS encoded string, we add 1 for the 0x00 delimiter

// Conversion macros (SYSTEM)
#define TICKS_TO_MS(time_ticks) ((time_ticks) * (1000 / osKernelSysTickFrequency)) // System ticks to milliseconds
#define MS_TO_TICKS(time_ms) ((time_ms) * (osKernelSysTickFrequency / 1000)) // Milliseconds to system ticks

// System Time Macros
constexpr uint32_t MAX_DELAY_MS = TICKS_TO_MS(portMAX_DELAY);
constexpr uint32_t MAX_DELAY_TICKS = portMAX_DELAY;

// Utility functions
namespace Utils
{
    // Arrays
    uint16_t AverageArray(uint16_t array[], int size);
    void WriteInt32ToArrayBigEndian(uint8_t* array, int startIndex, int32_t value);
    int32_t ReadInt32FromArrayBigEndian(uint8_t* array, int startIndex);

    // CRC
    uint32_t GetCRC32Aligned(uint8_t* data, uint32_t size);
    uint16_t GetCRC16(uint8_t* data, uint16_t size);

    bool IsCrc16Correct(uint8_t* data, uint16_t size, uint16_t crc);

    // String Manipulation
    inline bool IsAsciiNum(uint8_t c) { return (c >= '0' && c <= '9'); }
    inline bool IsAsciiChar(uint8_t c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
    inline bool IsAsciiLowercase(uint8_t c) { return (c >= 'a' && c <= 'z'); }

    // String to number conversion
    int32_t StringToLong(const char* str);

    // Debug functionality
    int32_t ExtractIntParameter(const char* msg, uint16_t identifierLen);


}


#endif    // CUBE_PLUSPLUS_INCLUDE_UTILS_HPP_
