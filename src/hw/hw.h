/*
 * hw.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#include "hw_def.h"

#include "led.h"
#include "uart.h"
#include "usb.h"
#include "cli.h"
#include "i2c.h"
#include "flash.h"
#include "sd.h"
#include "fatfs.h"
#include "files.h"
#include "timer.h"
#include "spi.h"
#include "gpio.h"
#include "sensors.h"
#include "gyro_init.h"
#include "gyro.h"
#include "bmi270.h"
#include "barometer.h"
#include "barometer_dps310.h"
#include "compass.h"
#include "compass_qmc5883l.h"
#include "acceleration.h"
#include "acceleration_init.h"
#include "gps.h"
#include "adc.h"
#include "rtc.h"


void hwInit(void);


#endif /* SRC_HW_HW_H_ */
