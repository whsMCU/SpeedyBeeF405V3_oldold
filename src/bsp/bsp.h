/*
 * bsp.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_


#include "def.h"
#include "stm32f4xx_hal.h"

extern uint8_t __config_start;   // configured via linker script when building binaries.
extern uint8_t __config_end;


#define _USE_LOG_PRINT    1

#if _USE_LOG_PRINT
#define logPrintf(fmt, ...)     printf(fmt, ##__VA_ARGS__)
#else
#define logPrintf(fmt, ...)
#endif

typedef enum {
    BOOTLOADER_REQUEST_ROM,
    BOOTLOADER_REQUEST_FLASH,
} bootloaderRequestType_e;

void systemReset(void);
void systemResetToBootloader(bootloaderRequestType_e requestType);
void checkForBootLoaderRequest(void);


typedef enum {
    FAILURE_DEVELOPER = 0,
    FAILURE_MISSING_ACC,
    FAILURE_ACC_INIT,
    FAILURE_ACC_INCOMPATIBLE,
    FAILURE_INVALID_EEPROM_CONTENTS,
    FAILURE_CONFIG_STORE_FAILURE,
    FAILURE_GYRO_INIT_FAILED,
    FAILURE_FLASH_READ_FAILED,
    FAILURE_FLASH_WRITE_FAILED,
    FAILURE_FLASH_INIT_FAILED, // RESERVED
    FAILURE_EXTERNAL_FLASH_READ_FAILED,  // RESERVED
    FAILURE_EXTERNAL_FLASH_WRITE_FAILED, // RESERVED
    FAILURE_EXTERNAL_FLASH_INIT_FAILED,
    FAILURE_SDCARD_READ_FAILED,
    FAILURE_SDCARD_WRITE_FAILED,
    FAILURE_SDCARD_INITIALISATION_FAILED,
    FAILURE_SDCARD_REQUIRED,
} failureMode_e;

void failureMode(failureMode_e mode);

void bspInit(void);
uint32_t getCycleCounter(void);

void HAL_SYSTICK_Callback(void);

int32_t clockCyclesToMicros(int32_t clockCycles);
int32_t clockCyclesTo10thMicros(int32_t clockCycles);
uint32_t clockMicrosToCycles(uint32_t micros);

void delay(uint32_t ms);
uint32_t millis(void);
uint32_t micros(void);
void delayMicroseconds(uint32_t us);

void initialiseMemorySections(void);

void Error_Handler(void);


#endif /* SRC_BSP_BSP_H_ */
