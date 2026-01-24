 /**
 ********************************************************************************
 * @file    DMATransfer.hpp
 * @author  Javier
 * @date    2026-01-10
 * @brief   Header for the DMA Transfer Driver.
 ********************************************************************************
 */

#ifndef CUBE_DMATRANSFER_HPP
#define CUBE_DMATRANSFER_HPP

/************************************
 * INCLUDES
 ************************************/
//#include "main.h"
#include "SystemDefines.hpp"
#include "cmsis_os.h"
#include <type_traits>

/************************************
 * CLASS DEFINITIONS
 ************************************/
class DMAControl {
public:
    template <typename HandleType>
    static HAL_StatusTypeDef Transfer(HandleType* handle, uint16_t devAddr, uint8_t* txData, uint8_t* rxData, uint16_t size) {
        
        // H7 / G4      Cache Management
        #if defined(STM32H7) || defined(STM32G4)
            // Cache To RAM before DMA
            if (txData != nullptr) SCB_CleanDCache_by_Addr((uint32_t*)txData, size);
            // Clean Cache to Force read RAM
            if (rxData != nullptr) SCB_InvalidateDCache_by_Addr((uint32_t*)rxData, size);
        #endif

        // ===   SPI Logic   ===
        if constexpr (std::is_same_v<HandleType, SPI_HandleTypeDef>) {
            // SPI Transfer (Full-Duplex)
            return HAL_SPI_TransmitReceive_DMA(handle, txData, rxData, size);
        }

        // ===   I2C Logic   ===
        if constexpr (std::is_same_v<HandleType, I2C_HandleTypeDef>) {
            
            // Transmit
            if (txData != nullptr && rxData == nullptr) {
                return HAL_I2C_Master_Transmit_DMA(handle, devAddr, txData, size);
            }
            
            // Receive
            else if (rxData != nullptr && txData == nullptr) {
                return HAL_I2C_Master_Receive_DMA(handle, devAddr, rxData, size);
            }
        }
            // ===   UART Logic   ===
        if constexpr (std::is_same_v<HandleType, UART_HandleTypeDef>) {
            // Transmit
            if (txData != nullptr && rxData == nullptr) {
                return HAL_UART_Transmit_DMA(handle, txData, size);
            }
            // Receive
            else if (rxData != nullptr && txData == nullptr) {
                return HAL_UART_Receive_DMA(handle, rxData, size);
            }
        }
        return HAL_ERROR;
    }
};

#endif /* CUBE_DMATRANSFER_HPP */