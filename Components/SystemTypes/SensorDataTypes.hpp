/**
 ********************************************************************************
 * @file    SensorDataTypes.hpp
 * @author  Shivam Desai
 * @date    Nov 3, 2024
 * @brief   General sensor data structure to pass around in the system or
 *log it to flash memory
 ********************************************************************************
 */

#ifndef SENSORDATATYPES_HPP_
#define SENSORDATATYPES_HPP_

#include <stdint.h>

/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/

/**
 * @param accelX The acceleration in the X axis relative to the sensor
 * @param accelY The acceleration in the Y axis relative to the sensor
 * @param accelZ The acceleration in the Z axis relative to the sensor
 */
struct ACCEL_t {
	int16_t x;
	int16_t y;
	int16_t z;
};

struct GYRO_t {
	int16_t x;
	int16_t y;
	int16_t z;
};

struct IMUData {
	ACCEL_t accel;
	GYRO_t gyro;
	int16_t temp;
	uint8_t id;
};


struct GPSData{
	uint32_t gps;
};


struct BaroData{
	int16_t temp;
	uint32_t pressure;
	uint8_t id;
};

struct FilterData{
	uint32_t alt;
	uint32_t velo;
	uint32_t accel;

	uint32_t altPredicted;
	uint32_t veloPredicted;
	uint32_t accelPredicted;
	uint32_t timePredicted;
};

struct MagData {
    uint32_t rawX;
    uint32_t rawY;
    uint32_t rawZ;
    float scaledX;
    float scaledY;
    float scaledZ;
};

#define NMEA_MAX_LENGTH 82

typedef struct
{
    int32_t    degrees_;
    int32_t    minutes_;
} LatLongType;

typedef struct
{
    int32_t     altitude_;
    char        unit_;
} AltitudeType;

typedef struct
{
    char            buffer_ [NMEA_MAX_LENGTH + 1];
    uint32_t        time_;
    LatLongType     latitude_;
    LatLongType     longitude_;
    AltitudeType    antennaAltitude_;
    AltitudeType    geoidAltitude_;
    AltitudeType    totalAltitude_;
} GpsData;

#endif /* SENSORDATATYPES_HPP_ */
