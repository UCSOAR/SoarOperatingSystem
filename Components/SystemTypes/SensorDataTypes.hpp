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



struct BaroData{
	int16_t temp;
	uint32_t pressure;
	uint8_t id;
};

/**
 * @brief Output of the altitude prediction filter (AltitudePredictionFilter/Output).
 *        Published on the DataBroker (FILTER_DATA) and logged to flash (LoggingData::FILTER).
 *        Altitudes are relative to the pad, up is positive
 * @param timestampMs     Frame time the estimate belongs to
 * @param altitude        m (UKF in flight, Dynamite before launch)
 * @param velocity        m/s
 * @param acceleration    m/s^2, net (gravity removed)
 * @param altitudeStd     m, 1 sigma
 * @param velocityStd     m/s, 1 sigma
 * @param predictedApogee m, 0 when there is no prediction
 * @param timeToEvent     s to the predicted apogee (before apogee) or landing (after apogee)
 * @param groundAltitude  m above sea level of the pad (tare)
 * @param apfState        ApfState (0 PRE_LAUNCH, 1 DETECTION, 2 LAUNCH, 3 LANDED)
 * @param flags           FILTER_FLAG_* bits (FilterOutput.hpp)
 * The rest are diagnostics (FILTER2 flash record, 1 Hz) and the flight event of this publish
 * (EVENT record, FilterLog::Event, 0 = none), layouts in AltitudePredictionFilter/Output/Inc/FilterLog.hpp
 */
struct FilterData{
	uint32_t timestampMs;
	float altitude;
	float velocity;
	float acceleration;
	float altitudeStd;
	float velocityStd;
	float predictedApogee;
	float timeToEvent;
	float groundAltitude;
	uint8_t apfState;
	uint8_t flags;

	int8_t caseA;			// Nearest scenario, -1 = UKF not running
	int8_t caseB;			// Second nearest
	float weightA;			// Blend weight of caseA (0..1)
	float innovationAlt;	// m, Dynamite altitude - UKF prediction
	float gainAlt;			// Kalman gain diagonal: 0 = follows the model, 1 = follows the sensors
	float gainVel;
	float gainAcc;
	float baroWeight[2];	// Dynamite trust of each baro (0..1)
	float dynamiteSigma;	// m, Dynamite fused altitude noise

	uint8_t event;			// FilterLog::Event, the LoggingTask writes it through to flash at once
	int32_t eventValue;		// Altitude in cm for flight events
};
struct MagDriverData{
	uint32_t rawX;
	uint32_t rawY;
	uint32_t rawZ;
	int32_t scaledX;
	int32_t scaledY;
	int32_t scaledZ;
};


struct MagData {
    int32_t magX;
    int32_t magY;
    int32_t magZ;
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
} GPSData;

typedef struct
{
    float     temp1;
    float 	  temp2;
    float	  temp3;
} ThermocoupleData;

typedef struct
{
    int32_t     pressure_1;
} PressureTransducerData;

typedef struct
{
    int32_t     pressure_1;
    int32_t     pressure_2;
} PBBPressureTransducerData;

#endif /* SENSORDATATYPES_HPP_ */
