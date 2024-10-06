# UART Driver

## Usage
### 1 Changing STM32 UART Line from HAL to UART
- First ensure the .ioc indicates the UART line is enabled
- (Note this was done in the Cube++ setup previously) Second go to the **Project Manager** tab at the top, under UART select the UART lines you want to use the driver and change them to use the LL library.
- Lastly ensure the UART Global Interrupt for any lines you want to use the driver for is enabled (TODO: Add screenshot)

### 2 Defining and Initializing UART Driver Instances

- Inside a .hpp file setup something like this, with names/number of drivers changed for however many uart lines you want
- I recommended setting up a section for aliases so you can reference a driver simply by doing UART::Debug-> for example
```C++
/* UART Driver Instances ------------------------------------------------------------------*/
class UARTDriver;

namespace Driver {
	extern UARTDriver lpuart1;
	extern UARTDriver uart1;
	extern UARTDriver uart2;
	extern UARTDriver uart3;
}

/* UART Driver Aliases ------------------------------------------------------------------*/
namespace UART {
	constexpr UARTDriver* RPI = &Driver::lpuart1; // UART Link to Raspberry Pi
	constexpr UARTDriver* SOB = &Driver::uart1;
	constexpr UARTDriver* Umbilical_DMB = &Driver::uart2;
	constexpr UARTDriver* Radio = &Driver::uart3;

	constexpr UARTDriver* Debug = &Driver::uart2;
}
```

Inside a `.cpp` file setup something like this, with names/number of drivers changed for however many uart lines you want
```C++
// Declare the global UART driver objects
namespace Driver {
    UARTDriver lpuart1(LPUART1);
    UARTDriver uart1(USART1);
    UARTDriver uart2(USART2);
    UARTDriver uart3(USART3);
}
```

Inside a RunInterface file or equivalent (a file that gives C functions access to C++ functions without name problems) define a function called
something like cpp_USART5_IRQHandler(), for example:
```C++
/*
 *  RunInterface.cpp
 */

#include "main_avionics.hpp"
#include "UARTDriver.hpp"

extern "C" {
    void run_interface()
    {
        run_main();
    }

    void cpp_USART5_IRQHandler()
    {
        Driver::uart5.HandleIRQ_UART();
    }
}

```
```C++

/*
 * RunInterface.hpp
 */

#ifndef C__IFACE_HPP_
#define C__IFACE_HPP_

void run_interface();

void cpp_USART5_IRQHandler();

#endif /* C__IFACE_HPP_ */

```

In this case you must call cpp_USART5_IRQHandler() inside Core/Src/stm32##xx_it.c where ## is the processor family. For example:
```C++
/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */
  cpp_USART2_IRQHandler();
  /* USER CODE END USART2_IRQn 1 */
}
```





