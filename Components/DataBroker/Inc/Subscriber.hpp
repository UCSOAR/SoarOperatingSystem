/**
 ********************************************************************************
 * @file    Subscriber.hpp
 * @author  Shivam Desai
 * @date    Nov 23, 2024
 * @brief
 ********************************************************************************
 */

#ifndef SUBSCRIBER_HPP_
#define SUBSCRIBER_HPP_

/************************************
 * INCLUDES
 ************************************/
#include "Task.hpp"
#include "SystemDefines.hpp"

/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * CLASS DEFINITIONS
 ************************************/
class Subscriber {
 public:
  void Init(Queue* subscriberQueueHandle) {
    if (taskQueue != nullptr) {
      SOAR_ASSERT(false, "You cannot overwrite a subscriber");
      return;
    }

    taskQueue = subscriberQueueHandle;
  }

  void Delete() {

    taskQueue = nullptr;
  }


  inline Queue* getSubscriberQueueHandle() const { return taskQueue; }

 private:

  Queue* taskQueue = nullptr;
};

/************************************
 * FUNCTION DECLARATIONS
 ************************************/

#endif /* SUBSCRIBER_HPP_ */
