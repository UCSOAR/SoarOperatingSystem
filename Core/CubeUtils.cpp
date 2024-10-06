/**
 ******************************************************************************
 * File Name          : Utils.cpp
 * Description        : Utility functions
 ******************************************************************************
*/
#include <cstring>

#include "cmsis_os.h"
#include "CubeUtils.hpp"
#include "etl/crc16_xmodem.h"
#include "SystemDefines.hpp"

/**
 * @brief Calculates the average from a list of unsigned shorts
 * @param array: The array of unsigned shorts to average
 * @param size: The size of the array
 * @return Returns the average as a uint16_t
 */
uint16_t Utils::AverageArray(uint16_t array[], int size)
{
    uint32_t sum = 0;

    for (int i = 0; i < size; i++)
    {
        sum += array[i];
    }

    return (sum / size);
}

/**
 * @brief converts an int32 to a uint8_t array in big endian format
 * right shift to put bytes in LSB slots, & with 0x00ff
 * @param array: The array to store the bytes in
 * @param startIndex: The index to start storing the bytes at
 * @param value: The int32 to convert
 */
void Utils::WriteInt32ToArrayBigEndian(uint8_t* array, int startIndex, int32_t value)
{
    array[startIndex + 0] = (value >> 24) & 0xFF;
    array[startIndex + 1] = (value >> 16) & 0xFF;
    array[startIndex + 2] = (value >> 8) & 0xFF;
    array[startIndex + 3] = value & 0xFF;
}

/**
 * @brief Converts a uint8_t array at a specific index stored in big endian form
 * @param array, the array read from the EEPROM 
 * @param startIndex, where the data field starts in the array
 * @return The value read from the array in big endian format
 */
int32_t Utils::ReadInt32FromArrayBigEndian(uint8_t* array, int startIndex)
{
    int32_t temp = 0;
    temp += (array[startIndex + 0] << 24); // eeprom reads little or big endian?
    temp += (array[startIndex + 1] << 16);
    temp += (array[startIndex + 2] << 8);
    temp += (array[startIndex + 3]);
    return temp;
}

/**
 * @brief Generates a CRC32 checksum for a given array of data using CRC Peripheral
 * @param data The data to generate the checksum for
 * @param size The size of the data array in uint8_t
 */
uint32_t Utils::GetCRC32Aligned(uint8_t* data, uint32_t size)
{
    // Figure out the number of bytes to pad by
    uint8_t pad = 0;

    // If the buffer is not a multiple of 4 bytes, then we need to pad the buffer by the remaining bytes
    if((size % 4) != 0)
        pad = 4 - (size % 4);

    // Generate a buffer padded to uint32_t
    uint32_t buffer[(size + pad) / 4];
    uint8_t* tempPtr = (uint8_t*)(&buffer[0]);

    // Bytewise copy and pad
    memcpy(tempPtr, data, size);
    memset(tempPtr + size, 0, pad);

    // Calculate the CRC32
    return HAL_CRC_Calculate(SystemHandles::CRC_Handle, (uint32_t*)buffer, (size+pad)/4);
}

/**
 * @brief Generates CRC16 checksum for a given array of data using etl::crc16
 * @param data The data to generate the checksum for
 * @param size  The size of the data array in uint8_t
 * @return The CRC16 checksum
 */
uint16_t Utils::GetCRC16(uint8_t* data, uint16_t size)
{
    // ETL CRC16 object
    etl::crc16_xmodem crc;
    crc.reset();

	// Use etl library to generate CRC16
	for (uint16_t i = 0; i < size; i++)
	{
        crc.add(data[i]);
	}

    return crc.value();
}

/**
 * @brief Checks if a given CRC is correct for the array
 * @param data The data (not including the checksum)
 * @param size The size of the data
 * @param crc The internal checksum
 * @return 
 */
bool Utils::IsCrc16Correct(uint8_t* data, uint16_t size, uint16_t crc)
{
    // First we calculate the crc16 for the buffer
    uint16_t calculatedCrc = GetCRC16(data, size);

    return (calculatedCrc == crc);
}

/**
 * @brief Converts a c string to a int32_t
 * @param str The string to convert, must be null terminated
 * @return The converted int32_t, or ERRVAL on an error
 */
int32_t Utils::StringToLong(const char* str)
{
    int32_t result = 0;
    const uint8_t size = (strlen(str) < 255) ? strlen(str) : 255;

    for (uint8_t i = 0; i < size; i++)
    {
        const uint8_t c = str[i];
        if (IsAsciiNum(c))
        {
            result *= 10;
            result += c - '0';
        }
        else
        {
            return ERRVAL;
        }
    }

    return result;
}

/**
 * @brief Extracts an integer parameter from a string
 * @brief msg Message to extract from, MUST be at least identifierLen long, and properly null terminated
 * @brief identifierLen Length of the identifier eg. 'rsc ' (Including the space) is 4
 * 
 * @example  if (strncmp(msg, "rsc ", 4) == 0) { // Notice the 4 here coresponds to the 4 in ExtractIntParameter
 *               // Get parameter
 *              int32_t state = ExtractIntParameter(msg, 4);
 *
 *              // Error check the state variable (confine to parameters)
 *              if (state != ERRVAL && state > 0 && state < UINT16_MAX) {
 *                  // Do something with the 'state' variable
 *              }
 *           }
 * 
 * @return ERRVAL on failure, otherwise the extracted value
 */
int32_t Utils::ExtractIntParameter(const char* msg, uint16_t identifierLen)
{
    // Handle a command with an int parameter at the end
    if (static_cast<uint16_t>(strlen(msg)) < identifierLen+1) {
        SOAR_PRINT("Int parameter insufficient length\r\n");
        return ERRVAL;
    }
    
    // Extract the value and attempt conversion to integer
    const int32_t val = Utils::StringToLong(&msg[identifierLen]);
    if (val == ERRVAL) {
        SOAR_PRINT("Int parameter invalid value\r\n");
    }

    return val;
}

