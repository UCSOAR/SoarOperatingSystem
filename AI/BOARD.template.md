# Board: <BOARD NAME>

> Copy this file to the board repo root as `BOARD.md` and fill it in. It is
> board-owned and is appended to the generated `AGENTS.md`. Keep it short:
> facts an AI agent needs that it cannot reliably work out from the code.

## Hardware

- MCU: <e.g. STM32H743VIT6>
- Purpose: <what this board does on the rocket / ground system>

## Peripherals and buses

| Peripheral | Instance | Connected to  |
| ---------- | -------- | ------------- |
| <SPI>      | <hspi2>  | <LSM6DSO IMU> |
| <UART>     | <UART8>  | <Debug>       |

## Tasks

| Task        | Purpose              |
| ----------- | -------------------- |
| <DebugTask> | <debug UART console> |

## Board-specific rules

- <e.g. Never change the RS485 timing constants without testing against the RCU>
