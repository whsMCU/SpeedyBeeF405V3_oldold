/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"
#include "uart.h"
#include "scheduler.h"
#include "tasks.h"
#include "rx/rx.h"
#include "pid_init.h"
#include "sensors.h"
#include "config/config.h"
#include "config/config_eeprom.h"
#include "pg/pg.h"
#include "mixer.h"
#include "adcinternal.h"
#include "stats.h"

  
void apInit(void)
{
	// Initialize task data as soon as possible. Has to be done before tasksInit(),
  // and any init code that may try to modify task behaviour before tasksInit().
  tasksInitData();

	cliOpen(_DEF_USB, 57600);

	initEEPROM();
	ensureEEPROMStructureIsValid();
  bool readSuccess = readEEPROM();

	if (!readSuccess || !isEEPROMVersionValid() || strncasecmp(systemConfig()->boardIdentifier, TARGET_BOARD_IDENTIFIER, sizeof(TARGET_BOARD_IDENTIFIER))) {
		resetEEPROM(false);
	}

	Sensor_Init();
	Baro_Init();
	compassInit();

	mixerInit(mixerConfig()->mixerMode);
	adcInternalInit();

    // Set the targetLooptime based on the detected gyro sampleRateHz and pid_process_denom
    gyroSetTargetLooptime(pidConfig()->pid_process_denom);

    // Validate and correct the gyro config or PID loop time if needed
    validateAndFixGyroConfig();

    // Now reset the targetLooptime as it's possible for the validation to change the pid_process_denom
    gyroSetTargetLooptime(pidConfig()->pid_process_denom);

	// Finally initialize the gyro filtering
	gyroInitFilters();
	pidInit(currentPidProfile);
	mixerInitProfile();
	rxInit();
    batteryInit(); // always needs doing, regardless of features.
	gpsInit();
	//gyroStartCalibration(false);
	//baroStartCalibration();
	#ifdef USE_PERSISTENT_STATS
		statsInit();
	#endif
	tasksInit();



}

void apMain(void)
{

	while(1)
	{
		scheduler();
	}
	// 	if (millis()-pre_time >= 1000)
    // 	{
    //  		pre_time = millis();
    //   		ledToggle(_DEF_LED1);
    // 	}
	// 	if (micros()-pre_time1 >= 312)
    // 	{
    //  		pre_time1 = micros();
	// 		imuUpdate();	//DT 170us
	// 		baroUpdate(micros());
    // 	}
	// 	if (millis()-pre_time2 >= 50)
    // 	{
    //  		pre_time2 = millis();
	// 		baroUpdate(micros());
	// 		calculateEstimatedAltitude(micros());
    // 	}
	// 	cliMain();
	// }
}
