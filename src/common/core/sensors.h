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

#define LPF_MAX_HZ 1000 // so little filtering above 1000hz that if the user wants less delay, they must disable the filter
#define DYN_LPF_MAX_HZ 1000

#define GYRO_LPF1_DYN_MIN_HZ_DEFAULT 250
#define GYRO_LPF1_DYN_MAX_HZ_DEFAULT 500
#define GYRO_LPF2_HZ_DEFAULT 500

#ifdef USE_YAW_SPIN_RECOVERY
#define YAW_SPIN_RECOVERY_THRESHOLD_MIN 500
#define YAW_SPIN_RECOVERY_THRESHOLD_MAX 1950
#endif

typedef enum {
    GYRO_HARDWARE_LPF_NORMAL,
    GYRO_HARDWARE_LPF_OPTION_1,
    GYRO_HARDWARE_LPF_OPTION_2,
#ifdef USE_GYRO_DLPF_EXPERIMENTAL
    GYRO_HARDWARE_LPF_EXPERIMENTAL,
#endif
    GYRO_HARDWARE_LPF_COUNT
} gyroHardwareLpf_e;

typedef struct {
    float w,x,y,z;
} quaternion;
#define QUATERNION_INITIALIZE  {.w=1, .x=0, .y=0,.z=0}

typedef struct {
    float ww,wx,wy,wz,xx,xy,xz,yy,yz,zz;
} quaternionProducts;
#define QUATERNION_PRODUCTS_INITIALIZE  {.ww=1, .wx=0, .wy=0, .wz=0, .xx=0, .xy=0, .xz=0, .yy=0, .yz=0, .zz=0}

typedef union {
    int16_t raw[XYZ_AXIS_COUNT];
    struct {
        // absolute angle inclination in multiple of 0.1 degree    180 deg = 1800
        int16_t roll;
        int16_t pitch;
        int16_t yaw;
    } values;
} attitudeEulerAngles_t;

typedef enum {
    GYRO_OVERFLOW_NONE = 0x00,
    GYRO_OVERFLOW_X = 0x01,
    GYRO_OVERFLOW_Y = 0x02,
    GYRO_OVERFLOW_Z = 0x04
} gyroOverflow_e;

#define EULER_INITIALIZE  { { 0, 0, 0 } }

#define GYRO_SCALE_2000DPS (2000.0f / (1 << 15))   // 16.384 dps/lsb scalefactor for 2000dps sensors
#define GYRO_SCALE_4000DPS (4000.0f / (1 << 15))   //  8.192 dps/lsb scalefactor for 4000dps sensors

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
void imuUpdateAttitude(uint32_t currentTimeUs);
void imuCalculateEstimatedAttitude(uint32_t currentTimeUs);
void DEBUG_print(void);

extern attitudeEulerAngles_t attitude;

#endif /* SRC_COMMON_CORE_SENSOR_H_ */
