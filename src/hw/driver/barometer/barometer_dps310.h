#ifndef SRC_COMMON_HW_INCLUDE_DSP310_H_
#define SRC_COMMON_HW_INCLUDE_DSP310_H_

#include "def.h"
#include "barometer.h"

bool dps310_Init(baroDev_t *baro);
bool dps310Detect(baroDev_t *baro);
bool dps310AddrRead(void);
bool dps310Read(baroDev_t *baro);
bool dps310SetCallBack(void (*p_func)(void));

#endif /* SRC_COMMON_HW_INCLUDE_DSP310_H_ */
