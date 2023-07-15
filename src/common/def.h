/*
 * def.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define _DEF_LED1           0
#define _DEF_LED2           1
#define _DEF_LED3           2
#define _DEF_LED4           3

#define _DEF_USB            0
#define _DEF_UART1          1
#define _DEF_UART2          2
#define _DEF_UART3          3
#define _DEF_UART4          4
#define _DEF_UART5          5
#define _DEF_UART6          6

#define _DEF_SPI1             0
#define _DEF_SPI2             1
#define _DEF_SPI3             2
#define _DEF_SPI4             3

#define _DEF_LOW              0
#define _DEF_HIGH             1

#define _DEF_INPUT            0
#define _DEF_INPUT_PULLUP     1
#define _DEF_INPUT_PULLDOWN   2
#define _DEF_INPUT_IT_RISING  3
#define _DEF_OUTPUT           4
#define _DEF_OUTPUT_PULLUP    5
#define _DEF_OUTPUT_PULLDOWN  6
#define _DEF_INPUT_AF_PP      7


#define MAX_SUPPORTED_MOTORS 8


#define USE_GYRO_OVERFLOW_CHECK
//#define USE_YAW_SPIN_RECOVERY
#define USE_ACC
#define USE_LAUNCH_CONTROL
#define USE_DYN_LPF
#define USE_D_MIN
#define USE_LATE_TASK_STATISTICS
#define USE_DYN_NOTCH_FILTER
#define USE_FAST_DATA
#define USE_SERIAL_RX
#define USE_ADC
#define CONFIG_IN_FLASH
//#define USE_CRSF_V3
//#define USE_CRSF_LINK_STATISTICS
//#define USE_DYN_IDLE

#ifdef USE_FAST_DATA
#define FAST_DATA_ZERO_INIT         __attribute__ ((section(".fastram_bss"), aligned(4)))
#define FAST_DATA                   __attribute__ ((section(".fastram_data"), aligned(4)))
#else
#define FAST_DATA_ZERO_INIT
#define FAST_DATA
#endif // USE_FAST_DATA


#endif /* SRC_COMMON_DEF_H_ */
