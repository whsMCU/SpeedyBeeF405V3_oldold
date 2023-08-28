/*
 * sensor.h
 *
 *  Created on: 2020. 12. 20.
 *      Author: WANG
 */

#ifndef SRC_COMMON_CORE_SENSOR_H_
#define SRC_COMMON_CORE_SENSOR_H_


#include "def.h"
#include "axis.h"
#include "maths.h"
#include "filter.h"
#include "gyro.h"

typedef enum {
    GYRO_OVERFLOW_NONE = 0x00,
    GYRO_OVERFLOW_X = 0x01,
    GYRO_OVERFLOW_Y = 0x02,
    GYRO_OVERFLOW_Z = 0x04
} gyroOverflow_e;

typedef enum {
    SENSOR_INDEX_GYRO = 0,
    SENSOR_INDEX_ACC,
    SENSOR_INDEX_BARO,
    SENSOR_INDEX_MAG,
    SENSOR_INDEX_RANGEFINDER,
    SENSOR_INDEX_COUNT
} sensorIndex_e;

extern uint8_t requestedSensors[SENSOR_INDEX_COUNT];
extern uint8_t detectedSensors[SENSOR_INDEX_COUNT];

typedef enum {
    SENSOR_GYRO = 1 << 0, // always present
    SENSOR_ACC = 1 << 1,
    SENSOR_BARO = 1 << 2,
    SENSOR_MAG = 1 << 3,
    SENSOR_SONAR = 1 << 4,
    SENSOR_RANGEFINDER = 1 << 4,
    SENSOR_GPS = 1 << 5,
    SENSOR_GPSMAG = 1 << 6
} sensors_e;

bool Sensor_Init(void);
void DEBUG_print(void);



#endif /* SRC_COMMON_CORE_SENSOR_H_ */
