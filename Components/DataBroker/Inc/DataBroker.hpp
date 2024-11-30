/**
 ********************************************************************************
 * @file    DataBroker.hpp
 * @author  shivam
 * @date    Nov 23, 2024
 * @brief
 ********************************************************************************
 */

#ifndef DATA_BROKER_HPP_
#define DATA_BROKER_HPP_

/************************************
 * INCLUDES
 ************************************/
#include "DataBroker.hpp"
#include "Publisher.hpp"
#include "SensorDataTypes.hpp"
#include "Command.hpp"
#include "DataBrokerMessageTypes.hpp"
#include "CubeDefines.hpp"
#include <type_traits>

/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/
template <typename T>
struct PublisherInformation {
  Publisher<T> publisher;
  DataBrokerMessageTypes messageType;
};

/************************************
 * CLASS DEFINITIONS
 ************************************/
class DataBroker {
 public:
  // Deleting the copy constructor to prevent copies
  DataBroker(const DataBroker& obj) = delete;

  // Deleting assignment operator to prevent assignment operations
  void operator=(DataBroker const&) = delete;

  // publish system message
  template <typename T>
  static void PublishData(T* dataToPublish) {
    PublisherInformation<T> publisherInformation = getPublisherInformation<T>();
    publisherInformation.publisher->Publish(dataToPublish, publisherInformation.messageType);
  }

 private:
  // matcher - match template type with publisher type
  template <typename T, typename U>
  static constexpr bool match() {
    return std::is_same_v<T, U>;
  }

  // get data publisher
  template <typename T>
  PublisherInformation<T> getPublisherInformation() {
    if constexpr (match<T, IMUData>()) {
      PublisherInformation<T> publisherInfo{.publisher = &IMU_Data_publisher,
                                            .messageType = DataBrokerMessageTypes::IMU_DATA};
      return publisherInfo;
    } else if constexpr (match<T, ThermocoupleData>()) {
      PublisherInformation<T> publisherInfo{.publisher = &Thermocouple_Data_publisher,
                                            .messageType = DataBrokerMessageTypes::THERMOCOUPLE_DATA};
      return publisherInfo;
    } else {
      SOAR_ASSERT(false, "This publisher type does not exist, you must create it");
    }
  }

  // list of publishers
  Publisher<IMUData> IMU_Data_publisher;
  Publisher<ThermocoupleData> Thermocouple_Data_publisher;
};
/************************************
 * FUNCTION DECLARATIONS
 ************************************/

#endif /* DATA_BROKER_HPP_ */
