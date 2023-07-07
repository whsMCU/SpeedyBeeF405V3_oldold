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

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "hw_def.h"

#include "core.h"
#include "gyro.h"
#include "pid.h"
#include "compass.h"
#include "acceleration.h"
#include "sensors.h"
#include "adc.h"
#include "dyn_notch_filter.h"
#include "rx.h"

typedef struct {
    uint32_t magic_number;
    char version_str[32];
} version_info_t;

typedef struct gyroConfig_s gyroConfig_t;
typedef struct pidProfile_s pidProfile_t;
typedef struct compassConfig_s compassConfig_t;
typedef struct accelerometerConfig_s accelerometerConfig_t;
typedef struct adcConfig_s adcConfig_t;
typedef struct dynNotchConfig_s dynNotchConfig_t;
typedef struct rxConfig_s rxConfig_t;

extern const version_info_t *p_verstion_info;
extern const gyroConfig_t *p_gyro_pg;
extern const pidProfile_t *p_pid_pg;
extern compassConfig_t *p_compass_pg;
extern accelerometerConfig_t *p_acc_pg;
extern adcConfig_t *p_adc_pg;
extern dynNotchConfig_t *p_notch_pg;
extern rxConfig_t *p_rx_pg;