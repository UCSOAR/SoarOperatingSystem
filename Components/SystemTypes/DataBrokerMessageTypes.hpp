/**
 ********************************************************************************
 * @file    DataBrokerMessageTypes.hpp
 * @author  Shivam Desai
 * @date    Nov 23, 2024
 * @brief
 ********************************************************************************
 */

#ifndef DATA_BROKER_MESSAGE_TYPES_HPP_
#define DATA_BROKER_MESSAGE_TYPES_HPP_

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>
#include <string>

/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/
enum class DataBrokerMessageTypes : uint8_t {
  INVALID = 0,
  IMU_DATA,
  GPS_DATA,
  BARO_DATA,
  FILTER_DATA,
  MAG_DATA,
};

namespace DataBrokerMessageType {
/************************************
 * CLASS DEFINITIONS
 ************************************/

/************************************
 * FUNCTION DECLARATIONS
 ************************************/
std::string ToString(DataBrokerMessageTypes messageType);

inline std::string ToString(DataBrokerMessageTypes messageType) {
  switch (messageType) {
    case DataBrokerMessageTypes::IMU_DATA: {
      std::string type{"IMU_DATA"};
      return type;
    }
    case DataBrokerMessageTypes::GPS_DATA: {
          std::string type{"GPS_DATA"};
          return type;
        }
    case DataBrokerMessageTypes::BARO_DATA: {
          std::string type{"BARO_DATA"};
          return type;
        }
    case DataBrokerMessageTypes::FILTER_DATA: {
          std::string type{"FILTER_DATA"};
          return type;
    }
    case DataBrokerMessageTypes::MAG_DATA: {
          std::string type{"MAG_DATA"};
          return type;
        }
    case DataBrokerMessageTypes::INVALID:
      [[fallthrough]];
    default: {
      std::string type{"INVALID"};
      return type;
    }
  }
}

}  // namespace DataBrokerMessageType

#endif /* DATA_BROKER_MESSAGE_TYPES_HPP_ */
