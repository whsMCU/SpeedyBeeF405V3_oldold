/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "barometer.h"
#include "barometer_dps310.h"
#include "maths.h"
#include "cli.h"
#include "i2c.h"
#include "scheduler/scheduler.h"

#ifdef USE_BARO_DPS310

#ifdef _USE_HW_CLI
static void cliDps310(cli_args_t *args);
#endif


baro_t baro;                        // barometer access functions

static uint16_t calibratingB = 0;      // baro calibration = get new ground pressure value
static int32_t baroPressure = 0;
static int32_t baroTemperature = 0;

static int32_t baroGroundAltitude = 0;
static int32_t baroGroundPressure = 8*101325;
static uint32_t baroPressureSum = 0;

uint8_t baro_sample_count = 21;              // size of baro filter array
uint16_t baro_noise_lpf = 600;                // additional LPF to reduce baro noise
uint16_t baro_cf_vel = 985;                   // apply Complimentary Filter to keep the calculated velocity based on baro velocity (i.e. near real velocity)

static int32_t estimatedAltitudeCm = 0;                // in cm

#define BARO_UPDATE_FREQUENCY_40HZ (1000 * 25)


#define CALIBRATING_BARO_CYCLES 200 // 10 seconds init_delay + 200 * 25 ms = 15 seconds before ground pressure settles
#define SET_GROUND_LEVEL_BARO_CYCLES 10 // calibrate baro to new ground level (10 * 25 ms = ~250 ms non blocking)

static bool baroReady = false;

void Baro_Init(void)
{
    dps310Detect(&baro.dev);
    baroStartCalibration();

    #ifdef _USE_HW_CLI
    cliAdd("dps310", cliDps310);
    #endif
}

bool baroIsCalibrationComplete(void)
{
    return calibratingB == 0;
}

static void baroSetCalibrationCycles(uint16_t calibrationCyclesRequired)
{
    calibratingB = calibrationCyclesRequired;
}

void baroStartCalibration(void)
{
    baroSetCalibrationCycles(CALIBRATING_BARO_CYCLES);
}

void baroSetGroundLevel(void)
{
    baroSetCalibrationCycles(SET_GROUND_LEVEL_BARO_CYCLES);
}

#define PRESSURE_SAMPLES_MEDIAN 3

static int32_t applyBarometerMedianFilter(int32_t newPressureReading)
{
    static int32_t barometerFilterSamples[PRESSURE_SAMPLES_MEDIAN];
    static int currentFilterSampleIndex = 0;
    static bool medianFilterReady = false;
    int nextSampleIndex;

    nextSampleIndex = (currentFilterSampleIndex + 1);
    if (nextSampleIndex == PRESSURE_SAMPLES_MEDIAN) {
        nextSampleIndex = 0;
        medianFilterReady = true;
    }

    barometerFilterSamples[currentFilterSampleIndex] = newPressureReading;
    currentFilterSampleIndex = nextSampleIndex;

    if (medianFilterReady)
        return quickMedianFilter3(barometerFilterSamples);
    else
        return newPressureReading;
}

static uint32_t recalculateBarometerTotal(uint32_t pressureTotal, int32_t newPressureReading)
{
    static int32_t barometerSamples[BARO_SAMPLE_COUNT_MAX + 1];
    static int currentSampleIndex = 0;
    int nextSampleIndex;

    // store current pressure in barometerSamples
    if (currentSampleIndex >= baro_sample_count) {
        nextSampleIndex = 0;
        baroReady = true;
    } else {
        nextSampleIndex = (currentSampleIndex + 1);
    }
    barometerSamples[currentSampleIndex] = applyBarometerMedianFilter(newPressureReading);

    // recalculate pressure total
    pressureTotal += barometerSamples[currentSampleIndex];
    pressureTotal -= barometerSamples[nextSampleIndex];

    currentSampleIndex = nextSampleIndex;

    return pressureTotal;
}

typedef enum {
    BARO_STATE_TEMPERATURE_READ = 0,
    BARO_STATE_TEMPERATURE_SAMPLE,
    BARO_STATE_PRESSURE_START,
    BARO_STATE_PRESSURE_READ,
    BARO_STATE_PRESSURE_SAMPLE,
    BARO_STATE_TEMPERATURE_START,
    BARO_STATE_COUNT
} barometerState_e;


bool isBaroReady(void) {
    return baroReady;
}

uint32_t baroUpdate(uint32_t currentTimeUs)
{
    static uint32_t baroStateDurationUs[BARO_STATE_COUNT];
    static barometerState_e state = BARO_STATE_PRESSURE_START;
    barometerState_e oldState = state;
    uint32_t executeTimeUs;
    uint32_t sleepTime = 1000; // Wait 1ms between states

    if (busBusy()){
        // If the bus is busy, simply return to have another go later
        schedulerIgnoreTaskStateTime();
        return sleepTime;
    }

    switch (state) {
        default:
        case BARO_STATE_TEMPERATURE_START:
            baro.dev.start_ut(&baro.dev);
            state = BARO_STATE_TEMPERATURE_READ;
            sleepTime = baro.dev.ut_delay;
            break;

        case BARO_STATE_TEMPERATURE_READ:
            if (baro.dev.read_ut(&baro.dev)) {
                state = BARO_STATE_TEMPERATURE_SAMPLE;
            } else {
                // No action was taken as the read has not completed
                schedulerIgnoreTaskExecTime();
            }
            break;

        case BARO_STATE_TEMPERATURE_SAMPLE:
            if (baro.dev.get_ut(&baro.dev)) {
                state = BARO_STATE_PRESSURE_START;
            } else {
                // No action was taken as the read has not completed
                schedulerIgnoreTaskExecTime();
            }
            break;

        case BARO_STATE_PRESSURE_START:
            baro.dev.start_up(&baro.dev);
            state = BARO_STATE_PRESSURE_READ;
            sleepTime = baro.dev.up_delay;
            break;

        case BARO_STATE_PRESSURE_READ:
            if (baro.dev.read_up(&baro.dev)) {
                state = BARO_STATE_PRESSURE_SAMPLE;
            } else {
                // No action was taken as the read has not completed
                schedulerIgnoreTaskExecTime();
            }
            break;

        case BARO_STATE_PRESSURE_SAMPLE:
            if (!baro.dev.get_up(&baro.dev)) {
                // No action was taken as the read has not completed
                schedulerIgnoreTaskExecTime();
                break;
            }

            baro.dev.calculate(&baroPressure, &baroTemperature);
            baro.baroPressure = baroPressure;
            baro.baroTemperature = baroTemperature;
            baroPressureSum = recalculateBarometerTotal(baroPressureSum, baroPressure);
            if (baro.dev.combined_read) {
                state = BARO_STATE_PRESSURE_START;
            } else {
                state = BARO_STATE_TEMPERATURE_START;
            }

            // DEBUG_SET(DEBUG_BARO, 1, baroTemperature);
            // DEBUG_SET(DEBUG_BARO, 2, baroPressure);
            // DEBUG_SET(DEBUG_BARO, 3, baroPressureSum);

            sleepTime = baro.dev.ut_delay;
            break;
    }

    // Where we are using a state machine call schedulerIgnoreTaskExecRate() for all states bar one
    if (sleepTime != baro.dev.ut_delay) {
        schedulerIgnoreTaskExecRate();
    }

    executeTimeUs = micros() - currentTimeUs;

    if (executeTimeUs > baroStateDurationUs[oldState]) {
        baroStateDurationUs[oldState] = executeTimeUs;
    }

    schedulerSetNextStateTime(baroStateDurationUs[state]);

    return sleepTime;
}

static float pressureToAltitude(const float pressure)
{
    return (1.0f - powf(pressure / 101325.0f, 0.190295f)) * 4433000.0f;
}

int32_t baroCalculateAltitude(void)
{
    int32_t BaroAlt_tmp;

    // calculates height from ground via baro readings
    if (baroIsCalibrationComplete()) {
        BaroAlt_tmp = lrintf(pressureToAltitude((float)(baroPressureSum / baro_sample_count)));
        BaroAlt_tmp -= baroGroundAltitude;
        baro.BaroAlt = lrintf((float)baro.BaroAlt * CONVERT_PARAMETER_TO_FLOAT(baro_noise_lpf) + (float)BaroAlt_tmp * (1.0f - CONVERT_PARAMETER_TO_FLOAT(baro_noise_lpf))); // additional LPF to reduce baro noise
    }
    else {
        baro.BaroAlt = 0;
    }
    return baro.BaroAlt;
}

void performBaroCalibrationCycle(void)
{
    static int32_t savedGroundPressure = 0;

    baroGroundPressure -= baroGroundPressure / 8;
    baroGroundPressure += baroPressureSum / baro_sample_count;
    baroGroundAltitude = (1.0f - pow_approx((baroGroundPressure / 8) / 101325.0f, 0.190259f)) * 4433000.0f;

    if (baroGroundPressure == savedGroundPressure) {
        calibratingB = 0;
    } else {
        calibratingB--;
        savedGroundPressure = baroGroundPressure;
    }
}

//static bool altitudeOffsetSetBaro = false;
//static bool altitudeOffsetSetGPS = false;

void calculateEstimatedAltitude(uint32_t currentTimeUs)
{
    static uint32_t previousTimeUs = 0;
    //static int32_t baroAltOffset = 0;
    //static int32_t gpsAltOffset = 0;

    const uint32_t dTime = currentTimeUs - previousTimeUs;
    if (dTime < BARO_UPDATE_FREQUENCY_40HZ) {
        schedulerIgnoreTaskExecTime();
        return;
    }
    previousTimeUs = currentTimeUs;

    int32_t baroAlt = 0;
    //int32_t gpsAlt = 0;
    //uint8_t gpsNumSat = 0;

    //float gpsTrust = 0.3; //conservative default
    bool haveBaroAlt = false;
    //bool haveGpsAlt = false;

    if (!baroIsCalibrationComplete()) {
        performBaroCalibrationCycle();
    } else {
        baroAlt = baroCalculateAltitude();
        haveBaroAlt = true;
    }


    // if (ARMING_FLAG(ARMED) && !altitudeOffsetSetBaro) {
    //     baroAltOffset = baroAlt;
    //     altitudeOffsetSetBaro = true;
    // } else if (!ARMING_FLAG(ARMED) && altitudeOffsetSetBaro) {
    //     altitudeOffsetSetBaro = false;
    // }

    // baroAlt -= baroAltOffset;

    estimatedAltitudeCm = baroAlt;
    baro.BaroAlt = baroAlt;

    //cliPrintf("BARO : %u cm \n\r", baroAlt);
    //DEBUG_SET(DEBUG_ALTITUDE, 0, (int32_t)(100 * gpsTrust));
    //DEBUG_SET(DEBUG_ALTITUDE, 1, baroAlt);
    //DEBUG_SET(DEBUG_ALTITUDE, 2, gpsAlt);
}

#ifdef _USE_HW_CLI
void cliDps310(cli_args_t *args)
{
  bool ret = false;

if (args->argc == 1 && args->isStr(0, "baro_show") == true)
{
    uint32_t pre_time;
    pre_time = millis();
    while(cliKeepLoop())
    {
        if (millis()-pre_time >= 1000)
        {
            pre_time = millis();
            // cliPrintf("acc x: %d, y: %d, z: %d\n\r", x, y, z);
        }
    }
    ret = true;
    }

  if (args->argc == 3 && args->isStr(0, "mem_read") == true)
  {
    uint8_t ch;
    uint8_t addr;

    ch   = (uint8_t)args->getData(1);
    addr = (uint8_t)args->getData(2);
    addr |= 0x80;

    ret = true;
  }

    if (args->argc == 4 && args->isStr(0, "mem_write") == true)
  {
    uint8_t ch;
    uint8_t addr;

    ch     = (uint8_t)args->getData(1);
    addr   = (uint8_t)args->getData(2);

    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("dsp310 baro_show \n\r");
    cliPrintf("dsp310 mem_read ch0:1, addr \n\r");
    cliPrintf("dsp310 mem_write ch0:1, addr data \n\r");
  }
}
#endif

#endif /* BARO */
