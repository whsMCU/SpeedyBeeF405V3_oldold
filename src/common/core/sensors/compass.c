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
#include <string.h>

//#if defined(USE_MAG)
#include "cli.h"
#include "scheduler.h"
#include "compass.h"
#include "compass_qmc5883l.h"

#ifdef _USE_HW_CLI
static void cliQmc5883l(cli_args_t *args);
#endif

static uint32_t tCal = 0;
static flightDynamicsTrims_t magZeroTempMin;
static flightDynamicsTrims_t magZeroTempMax;

magDev_t magDev;
mag_t mag;

static int16_t magADCRaw[XYZ_AXIS_COUNT];
static uint8_t magInit = 0;

void compassPreInit(void)
{
#ifdef USE_SPI
    if (compassConfig()->mag_busType == BUS_TYPE_SPI) {
        spiPreinitRegister(compassConfig()->mag_spi_csn, IOCFG_IPU, 1);
    }
#endif
}


bool compassDetect(magDev_t *magDev)
{
    magSensor_e magHardware = MAG_NONE;

#ifdef USE_MAG_DATA_READY_SIGNAL
    magDev->magIntExtiTag = compassConfig()->interruptTag;
#endif

    if (qmc5883lDetect(magDev)) {

        //*alignment = MAG_QMC5883L_ALIGN;

        magHardware = MAG_QMC5883;
    }

    if (magHardware == MAG_NONE) {
        return false;
    }

    // detectedSensors[SENSOR_INDEX_MAG] = magHardware;
    // sensorsSet(SENSOR_MAG);
    return true;
}

bool compassInit(void)
{
    // initialize and calibration. turn on led during mag calibration (calibration routine blinks it)

    //sensor_align_e alignment;

    if (!compassDetect(&magDev)) {
        return false;
    }

    //LED1_ON;
    magDev.init(&magDev);
    //LED1_OFF;
    magInit = 1;

    #ifdef _USE_HW_CLI
    cliAdd("qmc5883l", cliQmc5883l);
    #endif

    //magDev.magAlignment = alignment;

    // if (compassConfig()->mag_alignment != ALIGN_DEFAULT) {
    //     magDev.magAlignment = compassConfig()->mag_alignment;
    // }

    //buildRotationMatrixFromAlignment(&compassConfig()->mag_customAlignment, &magDev.rotationMatrix);

    return true;
}

bool compassIsHealthy(void)
{
    return (mag.magADC[X] != 0) && (mag.magADC[Y] != 0) && (mag.magADC[Z] != 0);
}

void compassStartCalibration(void)
{
    tCal = micros();
    flightDynamicsTrims_t *magZero = &p_compass_pg->magZero;
    for (int axis = 0; axis < 3; axis++) {
        magZero->raw[axis] = 0;
        magZeroTempMin.raw[axis] = mag.magADC[axis];
        magZeroTempMax.raw[axis] = mag.magADC[axis];
    }
}

bool compassIsCalibrationComplete(void)
{
    return tCal == 0;
}

uint32_t compassUpdate(uint32_t currentTimeUs)
{
    if (!magDev.read(&magDev, magADCRaw)) {
        // No action was taken as the read has not completed
        schedulerIgnoreTaskExecRate();
        return 1000; // Wait 1ms between states
    }

    for (int axis = 0; axis < XYZ_AXIS_COUNT; axis++) {
        mag.magADC[axis] = magADCRaw[axis];
    }
    // if (magDev.magAlignment == ALIGN_CUSTOM) {
    //     alignSensorViaMatrix(mag.magADC, &magDev.rotationMatrix);
    // } else {
    //     alignSensorViaRotation(mag.magADC, magDev.magAlignment);
    // }

    flightDynamicsTrims_t *magZero = &p_compass_pg->magZero;
    if (magInit) {              // we apply offset only once mag calibration is done
        mag.magADC[X] -= magZero->raw[X];
        mag.magADC[Y] -= magZero->raw[Y];
        mag.magADC[Z] -= magZero->raw[Z];
    }

    if (tCal != 0) {
        if ((currentTimeUs - tCal) < 30000000) {    // 30s: you have 30s to turn the multi in all directions
            //LED0_TOGGLE;
            for (int axis = 0; axis < 3; axis++) {
                if (mag.magADC[axis] < magZeroTempMin.raw[axis])
                    magZeroTempMin.raw[axis] = mag.magADC[axis];
                if (mag.magADC[axis] > magZeroTempMax.raw[axis])
                    magZeroTempMax.raw[axis] = mag.magADC[axis];
            }
        } else {
            tCal = 0;
            for (int axis = 0; axis < 3; axis++) {
                magZero->raw[axis] = (magZeroTempMin.raw[axis] + magZeroTempMax.raw[axis]) / 2; // Calculate offsets
            }

            //saveConfigAndNotify();
        }
    }

    return TASK_PERIOD_HZ(10);
}

#ifdef _USE_HW_CLI
void cliQmc5883l(cli_args_t *args)
{
  bool ret = false;

if (args->argc == 1 && args->isStr(0, "compass_show") == true)
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
    cliPrintf("qmc5883l show \n\r");
    cliPrintf("qmc5883l mem_read ch0:1, addr \n\r");
    cliPrintf("qmc5883l mem_write ch0:1, addr data \n\r");
  }
}
#endif

//#endif // USE_MAG
