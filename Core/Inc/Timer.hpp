/**
 ******************************************************************************
 * File Name          : Timer.hpp
 * Description        : Wrapper for FreeRTOS Timers
 ******************************************************************************
*/
#ifndef AVIONICS_INCLUDE_SOAR_CORE_TIMER_H
#define AVIONICS_INCLUDE_SOAR_CORE_TIMER_H
/* Includes ------------------------------------------------------------------*/
#include <CubeUtils.hpp>
#include "CubeRTOS.hpp"    // FreeRTOS kernel API (CMSIS_V1/V2 agnostic)
#include "FreeRTOS.h"

/* Macros --------------------------------------------------------------------*/
constexpr uint32_t DEFAULT_TIMER_COMMAND_WAIT_PERIOD = MS_TO_TICKS(15); // Default time to block a task if a command cannot be issued to the timer

#define DEFAULT_TIMER_PERIOD (MS_TO_TICKS(1000)) // 1s

// Enumeration representing the 4 timer states
enum TimerState {
    UNINITIALIZED=0,
    COUNTING,
    PAUSED,
    COMPLETE
};


/* Class -----------------------------------------------------------------*/

/**
 * @brief Timer class
 *
 * Wrapper for FreeRTOS Timers
*/
class Timer
{
public:
    Timer(); // Default Constructor (Polling Timer)
    Timer(void (*TimerCallbackFunction_t)( TimerHandle_t xTimer )); // Constructor for Callback Enabled Timer
    ~Timer();
    bool ChangePeriodMs(const uint32_t period_ms); // Resets timers and initializes period to specified parameters
    bool ChangePeriodMsAndStart(const uint32_t period_ms); // Restarting timer with the specified parameter
    bool Start();
    bool Stop();
    bool ResetTimer();
    bool ResetTimerAndStart();


    const uint32_t GetOriginalPeriodMs(){return timerPeriod;};

    const TimerState GetState(); // Returns state of the timer
    const uint32_t GetPeriodMs(); // Returns period in ms
    const uint32_t GetRemainingTimeMs(); // Returns time left till timer will expire

#if (tskKERNEL_VERSION_MAJOR >= 10 && tskKERNEL_VERSION_MINOR >= 3)
    void SetAutoReload(bool setReloadOn);  //True for Autoreload and False for One-shot
    const bool GetIfAutoReload(); // Returns true if timer is Autoreload and False if it is One-shot
#endif

    static void DefaultCallback( TimerHandle_t xTimer );

protected:
    const uint32_t GetRTOSTimeRemaining();

    TimerState timerState; // Enum that holds current timer state
    TimerHandle_t rtTimerHandle;
    uint32_t timerPeriod = DEFAULT_TIMER_PERIOD;
    uint32_t remainingTimeBetweenPauses; // Calculates time left on timer when it is paused

};



#endif /* AVIONICS_INCLUDE_SOAR_CORE_TIMER_H*/
