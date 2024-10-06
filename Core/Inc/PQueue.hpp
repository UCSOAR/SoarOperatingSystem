/**
 ******************************************************************************
 * File Name          : PQueue.hpp
 * 
 * Configuration      : Define macros in SystemDefines.hpp
 *    #define PQUEUE_ERROR_COUNT_MAX <int> - Max errors before assert
 *    #define PQUEUE_SEQN_TYPE <type> - Sequence number type
 *    #define PQUEUE_DISABLE_SEQN_CIRCULAR_CHECK - Disable 
 *      sequence number circular check, saves +4/8B (pointer size) overhead
 *      per item, but reduces protection against sequence number wrap-around
 * 
 * Description        :
 *    PQueue is a wrapper for FreeRTOS Queues and ETL Priority Queues to
 *    allow proper signaling within FreeRTOS
 * 
 *    This priority queue is intended to be used for task event signaling and
 *    prioritization and maintains FIFO ordering within each priority level.
 * 
 *    If FIFO ordering and RTOS signaling is not required, consider using the
 *    ETL priority queue directly.
 * 
 *    Note: In order to maintain FIFO ordering, a sequence number is used in 
 *    each queue item. Note that the sequence number will wrap around. This is
 *    handled by the comparison operator in the PriorityQueueItem struct,
 *    however it assumes that the sequence number will not wrap around twice
 *    before all items in the previous wrap-around have been received from 
 *    the queue. By default a 16-bit sequence number is used, which should
 *    be a reasonable trade-off for most applications. The sequence number type
 *    can be configured by defining PQUEUE_SEQN_TYPE in SystemDefines
 * 
 * Future Improvements :
 *   - Implementing the priority queue directly (not ETL) would allow the sequence
 *   number check to be integrated into the class itself, removing the per-item
 *   overhead of the sequence number pointer.
 ******************************************************************************
*/
#ifndef CUBE_PLUSPLUS_INCLUDE_PRIORITY_QUEUE_H
#define CUBE_PLUSPLUS_INCLUDE_PRIORITY_QUEUE_H

/* Includes ------------------------------------------------------------------*/
#include "etl/priority_queue.h"
#include "TQueue.hpp"
#include "Mutex.hpp"
#include "CubeDefines.hpp"
#include "SystemDefines.hpp"

/* Constants and Definitions -------------------------------------------------*/
constexpr uint32_t PQUEUE_MTX_TIMEOUT_MS = 250; // Mutex Timeout
constexpr uint8_t RTQUEUE_ITEM = 1; // RTOS Queue Holder Item
enum Priority : uint8_t {
    HIGH = 200,   // 200 so +- ~50 for fine adjustment
    NORMAL = 127, // 127 centered
    LOW = 50,     // 50 so +- ~50 for fine adjustment

    // Alias enumerations
    MID = NORMAL,
};

/* User Configurable Defines -------------------------------------------------*/
#ifndef PQUEUE_ERROR_COUNT_MAX // Maximum number of errors before assert
#define PQUEUE_ERROR_COUNT_MAX 10
#endif
#ifndef PQUEUE_SEQN_TYPE // Sequence number type
#define PQUEUE_SEQN_TYPE uint16_t
#endif
#ifndef PQUEUE_DISABLE_SEQN_CIRCULAR_CHECK // Disable sequence number circular check 
#define PQUEUE_ENABLE_SEQN_CIRCULAR_CHECK // (uses +4/8B (pointer size) overhead per item)
#endif

/* Macros --------------------------------------------------------------------*/
typedef PQUEUE_SEQN_TYPE seq_t;

/* Class ---------------------------------------------------------------------*/
/**
 * @brief Priority Queue Class
 * 
 * @tparam T Object for the priority queue
 * @tparam SIZE Depth of the priority queue in number of objects
 */
template<typename T, const size_t SIZE>
class PQueue {
public:
    PQueue();

    bool Send(const T& item, uint8_t priority = Priority::NORMAL); // Intentionally uint8_t to allow Priority::NORMAL+1 for example
    bool Receive(T& item, uint32_t timeout_ms = 0);
    bool ReceiveWait(T& item);

    bool IsEmpty() const { return rtQueue_.IsEmpty(); }
    bool IsFull() const { return rtQueue_.IsFull(); }
    uint16_t GetCurrentCount() const { return rtQueue_.GetQueueMessageCount(); }
    uint16_t GetMaxDepth() const { return rtQueue_.GetQueueDepth(); }

private:
    void HandleConsistencyError();
    void NotifySelf() { uint8_t item = RTQUEUE_ITEM; rtQueue_.Send(item); }

private:
    struct PriorityQueueItem {
        T data_;
        uint8_t priority_;
        seq_t order_;
#ifdef PQUEUE_ENABLE_SEQN_CIRCULAR_CHECK
        const seq_t* pSeqN_;
#endif

        bool operator<(const PriorityQueueItem& other) const {
            if(priority_ == other.priority_) {
#ifdef PQUEUE_ENABLE_SEQN_CIRCULAR_CHECK
                SOAR_ASSERT(pSeqN_ != nullptr, "PQueue null seqn pointer");
                seq_t seqN = *pSeqN_;
                if((order_ < seqN && other.order_ < seqN) ||
                   (order_ >= seqN && other.order_ >= seqN)) {
                    // Both are before wrap-around or after wrap-around
                    return order_ > other.order_;
                }
                else if(order_ < seqN) {
                    // This is after wrap-around, other is before
                    return true;
                }
                else {
                    // This is before wrap-around, other is after
                    return false;
                }
#else
                return order_ > other.order_;
#endif
            }
            else {
                return priority_ < other.priority_;
            }
        }
    };

    TQueue<uint8_t> rtQueue_;
    etl::priority_queue<PriorityQueueItem, SIZE> etlQueue_;
    Mutex mtx_;
    seq_t seqN_;

    uint8_t errCount_;
};

/* Functions ---------------------------------------------------------------------*/
/**
 * @brief Construct a new PQueue<T, SIZE>::PQueue object
 * 
 * @tparam T Object for the priority queue
 * @tparam SIZE Depth of the priority queue in N objects
 */
template<typename T, const size_t SIZE>
PQueue<T, SIZE>::PQueue() : 
    rtQueue_(SIZE)
 {
    errCount_ = 0;
    seqN_ = 0;
 }

/**
 * Sends an item with a specified priority to the priority queue.
 *
 * @param item The item to be sent to the priority queue.
 * @param priority The priority of the item.
 *
 * @return True if the item was successfully sent, false otherwise.
 */
template<typename T, const size_t SIZE>
bool PQueue<T, SIZE>::Send(const T& item, uint8_t priority) {
    // If we cannot acquire the priority queue mutex, do nothing
    if(!mtx_.Lock(PQUEUE_MTX_TIMEOUT_MS)) {
        return false;
    }

    // If the queue is full we cannot do anything
    if (etlQueue_.full()) {
        return false;
    }

    // Push an item to the priority queue
#ifdef PQUEUE_ENABLE_SEQN_CIRCULAR_CHECK
    etlQueue_.push({item, priority, seqN_, &seqN_});
#else
    etlQueue_.push({item, priority, seqN_});
#endif

    // Update the sequence number
    seqN_ += 1;

    // Push an item to the RTOS queue
    NotifySelf();

    // Unlock the priority queue mutex
    mtx_.Unlock();

    return true;
}

/**
 * Receives an item from the priority queue.
 *
 * @param item the item to be received into from the priority queue
 * @param timeout_ms the timeout in milliseconds to wait for an item to be available in the queue
 *
 * @return true if an item was successfully received, false otherwise
 */
template<typename T, const size_t SIZE>
bool PQueue<T, SIZE>::Receive(T& item, uint32_t timeout_ms) {
    // RTOS Queue Poll, if no item, return false
    uint8_t rtqItem;
    if(!rtQueue_.Receive(rtqItem, timeout_ms)) {
        return false;
    }

    // If we failed to acquire the priority queue mutex, you must add another item to the rtos queue to ensure size consistency
	if(!mtx_.Lock(PQUEUE_MTX_TIMEOUT_MS)) {
		NotifySelf();
		return false;
	}

    // If there is an empty etlQueue after getting a rtQueue_ response, we have a queue consistency error
    if (etlQueue_.empty()) {
        HandleConsistencyError();
        return false;
    }
    
    // Get the item from the etlQueue and pop it
    item = etlQueue_.top().data_;
    etlQueue_.pop();

    // If the queue is now empty, we can reset the sequence number
    if(IsEmpty()) { 
        seqN_ = 0;
    }

    // Unlock the priority queue mutex
    mtx_.Unlock();

    return true;
}

/**
 * Wait forever for an item to be available in the priority queue.
 *
 * @param item the item to be received into from the priority queue
 *
 * @return true if the item was successfully received, false otherwise
 */
template<typename T, const size_t SIZE>
bool PQueue<T, SIZE>::ReceiveWait(T& item) {
    return Receive(item, TICKS_TO_MS(HAL_MAX_DELAY));
}

/**
 * Handles a consistency error in the PQueue class.
 *
 * Prints an error message through debug. Then, it counts the number of errors 
 * and checks if it exceeds the maximum limit. If the error count exceeds the limit, the function 
 * fails an assert.
 * 
 * In the event of a consistency error, this function will pop and add items to the RT queue until
 * it matches that of the priority queue.
 *
 * @tparam T The type of elements in the priority queue.
 * @tparam SIZE The maximum size of the priority queue.
 *
 * @return true if the consistency error was handled successfully, false otherwise.
 */
template<typename T, const size_t SIZE>
void PQueue<T, SIZE>::HandleConsistencyError() {
    // Print an error
    SOAR_PRINT("ERROR: PQueue Data Consistency\r\n");

    // Count the error, if it exceeds the max, we must reset the system
    SOAR_ASSERT(++errCount_ > PQUEUE_ERROR_COUNT_MAX,
			"PQueue data consistency faults exceeded limits");

    // Pop/Add items to the RT queue until it matches that of the priority queue
    uint16_t pQueueSize = etlQueue_.size();
    uint16_t rtQueueSize = rtQueue_.GetQueueMessageCount();
    if(pQueueSize == rtQueueSize) {
        // Size consistent, do nothing
        --errCount_;
        return;
    }
    else if(pQueueSize > rtQueueSize) {
        // Add items until we reach the size of the priority queue
        for(uint16_t i = rtQueueSize; i < pQueueSize; i++) {
            NotifySelf();
        }
    }
    else {
        // Remove items until we reach the size of the priority queue
        for(uint16_t i = pQueueSize; i < rtQueueSize; i++) {
            uint8_t item;
            rtQueue_.Receive(item);
        }
    }
}

#endif // CUBE_PLUSPLUS_INCLUDE_PRIORITY_QUEUE_H
