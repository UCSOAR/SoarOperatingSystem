# DMA Transfer Driver

**Author:** Javier  
**Date:** 2026-01-10  
**Status:** Active  

## Overview

The **DMA Transfer Driver** is a header-only abstraction layer designed to simplify Direct Memory Access (DMA) transfers for STM32 peripherals. It provides a single, unified static interface (`DMAControl::Transfer`) that automatically handles protocol-specific HAL calls for **SPI**, **I2C**, and **UART**.

This driver also manages **Cache Coherence** for Cortex-M7 (STM32H7) and Cortex-M4 (STM32G4) devices, ensuring that data located in D-Cache is properly cleaned (flushed) to RAM before transmission or invalidated (refreshed) from RAM after reception.

## Features

* **Unified Interface:** Uses C++ `if constexpr` templates to detect the handle type (`SPI_HandleTypeDef`, `I2C_HandleTypeDef`, `UART_HandleTypeDef`) and call the matching HAL function.
* **Cache Management:** Automatically calls `SCB_CleanDCache_by_Addr` and `SCB_InvalidateDCache_by_Addr` if the architecture requires it (H7/G4).
* **Zero-Overhead:** Being header-only template library, the compiler optimizes unused branches, resulting in no runtime performance penalty compared to raw HAL calls.

## Directory Structure

* **Header:** `SoarOS/Drivers/Inc/DMATransfer.hpp`
* **Documentation:** `SoarOS/Drivers/DMATransfer_README.md`

## API Reference

```cpp
static HAL_StatusTypeDef Transfer(
    HandleType* handle,   // Pointer to the HAL handle (e.g., &hspi1, &hi2c1, &huart2)
    uint16_t devAddr,     // Device Address (Used for I2C only; set to 0 for others)
    uint8_t* txData,      // Pointer to Transmit Buffer (nullptr if receive-only)
    uint8_t* rxData,      // Pointer to Receive Buffer (nullptr if transmit-only)
    uint16_t size         // Number of bytes to transfer
);
```

## Usage

### Include the Header

```cpp
#include "DMATransfer.hpp"
```

### I2C

```cpp
// Example: Write 4 bytes to an EEPROM at address 0x50
uint8_t data[] = {0x00, 0x01, 0xAA, 0xBB};
DMAControl::Transfer(&hi2c1, (0x50 << 1), data, nullptr, 4);

// Example: Read 10 bytes from a sensor at address 0x68
uint8_t rxBuffer[10];
DMAControl::Transfer(&hi2c1, (0x68 << 1), nullptr, rxBuffer, 10);
```

### SPI

```cpp
uint8_t txBuf[] = {0xCA, 0xFE};
uint8_t rxBuf[2];

// Full Duplex Transfer
DMAControl::Transfer(&hspi1, 0, txBuf, rxBuf, 2);
```

### UART

```cpp
// Transmit Message via DMA
uint8_t msg[] = "Hello World";
DMAControl::Transfer(&huart1, 0, msg, nullptr, 11);

// Receive Data via DMA
uint8_t inputBuf[64];
DMAControl::Transfer(&huart1, 0, nullptr, inputBuf, 64);
```

### NOTES

* This Driver Requires HAL handles. It is not directly compatible with LL pointers without a wrapper.
* DMA channels should be enabled and linked to peripherals in main.c / msp.c before calling this function.
