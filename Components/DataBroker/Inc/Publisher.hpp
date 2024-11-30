/**
 ********************************************************************************
 * @file    Publisher.hpp
 * @author  shivam
 * @date    Nov 23, 2024
 * @brief
 ********************************************************************************
 */

#ifndef PUBLISHER_HPP_
#define PUBLISHER_HPP_

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>
#include <array>
#include "Task.hpp"
#include "Subscriber.hpp"
#include "DataBrokerMessageTypes.hpp"

/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * CLASS DEFINITIONS
 ************************************/
template <typename T, uint8_t MaxSubscribers = 5>
class Publisher {
 public:
  // Constructor
  Publisher();

  // subscribe
  void Subscribe(Task* taskToSubscribe);

  // unsubscribe
  void Unsubscribe(Task* taskToUnsubscribe);

  // publish
  void Publish(T* dataToPublish, DataBrokerMessageTypes messageType);

 private:
  // list of subscribers
  Subscriber subscribersList[MaxSubscribers] = {};
};

/************************************
 * FUNCTION DECLARATIONS
 ************************************/

#endif /* PUBLISHER_HPP_ */
