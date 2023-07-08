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
#include <stdlib.h>

#include <string.h>

#include "pg/pg.h"
#include "pg/pg_ids.h"
#include "pg/rx.h"


#include "rx/rx.h"
#include "sbus.h"
#include "tasks.h"
#include "maths.h"
#include "utils.h"
#include "axis.h"
#include "uart.h"
#include "cli.h"
#include "crsf.h"


float rcCommand[4];           // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW
static bool isRxDataNew = false;
static inline int32_t cmpTimeUs(uint32_t a, uint32_t b) { return (int32_t)(a - b); }

const char rcChannelLetters[] = "AERT12345678abcdefgh";
#define RX_MAPPABLE_CHANNEL_COUNT 8
uint8_t rcmap[RX_MAPPABLE_CHANNEL_COUNT];

static uint16_t rssi = 0;                  // range: [0;1023]
static int16_t rssiDbm = -130;//CRSF_RSSI_MIN;    // range: [-130,20]
static uint32_t lastMspRssiUpdateUs = 0;

//static pt1Filter_t frameErrFilter;

#ifdef USE_RX_LINK_QUALITY_INFO
static uint16_t linkQuality = 0;
static uint8_t rfMode = 0;
#endif

#ifdef USE_RX_LINK_UPLINK_POWER
static uint16_t uplinkTxPwrMw = 0;  //Uplink Tx power in mW
#endif

#define RSSI_ADC_DIVISOR (4096 / 1024)
#define RSSI_OFFSET_SCALING (1024 / 100.0f)

rssiSource_e rssiSource;
linkQualitySource_e linkQualitySource;

static bool rxDataProcessingRequired = false;
static bool auxiliaryProcessingRequired = false;

static bool rxSignalReceived = false;
static bool rxFlightChannelsValid = false;
static uint8_t rxChannelCount;

static uint32_t needRxSignalBefore = 0;
static uint32_t suspendRxSignalUntil = 0;
static uint8_t  skipRxSamples = 0;

static float rcRaw[MAX_SUPPORTED_RC_CHANNEL_COUNT];     // last received raw value, as it comes
float rcData[MAX_SUPPORTED_RC_CHANNEL_COUNT];           // scaled, modified, checked and constrained values
uint32_t validRxSignalTimeout[MAX_SUPPORTED_RC_CHANNEL_COUNT];

#define MAX_INVALID_PULSE_TIME_MS 300                   // hold time in milliseconds after bad channel or Rx link loss
// will not be actioned until the nearest multiple of 100ms
#define PPM_AND_PWM_SAMPLE_COUNT 3

#define DELAY_20_MS (20 * 1000)                         // 20ms in us
#define DELAY_100_MS (100 * 1000)                       // 100ms in us
#define DELAY_1500_MS (1500 * 1000)                     // 1.5 seconds in us
#define SKIP_RC_SAMPLES_ON_RESUME  2                    // flush 2 samples to drop wrong measurements (timing independent)

rxRuntimeState_t rxRuntimeState;
static uint8_t rcSampleIndex = 0;

// PG_REGISTER_ARRAY_WITH_RESET_FN(rxChannelRangeConfig_t, NON_AUX_CHANNEL_COUNT, rxChannelRangeConfigs, PG_RX_CHANNEL_RANGE_CONFIG, 0);
// void pgResetFn_rxChannelRangeConfigs(rxChannelRangeConfig_t *rxChannelRangeConfigs)
// {
//     // set default calibration to full range and 1:1 mapping
//     for (int i = 0; i < NON_AUX_CHANNEL_COUNT; i++) {
//         rxChannelRangeConfigs[i].min = PWM_RANGE_MIN;
//         rxChannelRangeConfigs[i].max = PWM_RANGE_MAX;
//     }
// }

// PG_REGISTER_ARRAY_WITH_RESET_FN(rxFailsafeChannelConfig_t, MAX_SUPPORTED_RC_CHANNEL_COUNT, rxFailsafeChannelConfigs, PG_RX_FAILSAFE_CHANNEL_CONFIG, 0);
// void pgResetFn_rxFailsafeChannelConfigs(rxFailsafeChannelConfig_t *rxFailsafeChannelConfigs)
// {
//     for (int i = 0; i < MAX_SUPPORTED_RC_CHANNEL_COUNT; i++) {
//         rxFailsafeChannelConfigs[i].mode = (i < NON_AUX_CHANNEL_COUNT) ? RX_FAILSAFE_MODE_AUTO : RX_FAILSAFE_MODE_HOLD;
//         rxFailsafeChannelConfigs[i].step = (i == THROTTLE)
//             ? CHANNEL_VALUE_TO_RXFAIL_STEP(RX_MIN_USEC)
//             : CHANNEL_VALUE_TO_RXFAIL_STEP(RX_MID_USEC);
//     }
// }

void resetAllRxChannelRangeConfigurations(rxChannelRangeConfig_t *rxChannelRangeConfig) {
    // set default calibration to full range and 1:1 mapping
    for (int i = 0; i < NON_AUX_CHANNEL_COUNT; i++) {
        rxChannelRangeConfig->min = PWM_RANGE_MIN;
        rxChannelRangeConfig->max = PWM_RANGE_MAX;
        rxChannelRangeConfig++;
    }
}

static float nullReadRawRC(const rxRuntimeState_t *rxRuntimeState, uint8_t channel)
{
    UNUSED(rxRuntimeState);
    UNUSED(channel);

    return 0; //PPM_RCVR_TIMEOUT
}

static uint8_t nullFrameStatus(rxRuntimeState_t *rxRuntimeState)
{
    UNUSED(rxRuntimeState);

    return RX_FRAME_PENDING;
}

static bool nullProcessFrame(const rxRuntimeState_t *rxRuntimeState)
{
    UNUSED(rxRuntimeState);

    return true;
}

bool isPulseValid(uint16_t pulseDuration)
{
    return  pulseDuration >= 885 &&
            pulseDuration <= 2115;
}

#define USE_SERIAL_RX
#define USE_SERIALRX_SBUS
#define USE_SERIALRX_CRSF
#ifdef USE_SERIAL_RX
static bool serialRxInit(rxRuntimeState_t *rxRuntimeState)
{
    bool enabled = false;
    switch (rxRuntimeState->serialrxProvider) {
#ifdef USE_SERIALRX_SRXL2
    case SERIALRX_SRXL2:
        enabled = srxl2RxInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_SPEKTRUM
    case SERIALRX_SRXL:
    case SERIALRX_SPEKTRUM1024:
    case SERIALRX_SPEKTRUM2048:
        enabled = spektrumInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_SBUS
    case SERIALRX_SBUS:
        enabled = sbusInit(rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_SUMD
    case SERIALRX_SUMD:
        enabled = sumdInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_SUMH
    case SERIALRX_SUMH:
        enabled = sumhInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_XBUS
    case SERIALRX_XBUS_MODE_B:
    case SERIALRX_XBUS_MODE_B_RJ01:
        enabled = xBusInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_IBUS
    case SERIALRX_IBUS:
        enabled = ibusInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_JETIEXBUS
    case SERIALRX_JETIEXBUS:
        enabled = jetiExBusInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_CRSF
    case SERIALRX_CRSF:
        enabled = crsfRxInit(rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_GHST
    case SERIALRX_GHST:
        enabled = ghstRxInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_TARGET_CUSTOM
    case SERIALRX_TARGET_CUSTOM:
        enabled = targetCustomSerialRxInit(rxConfig, rxRuntimeState);
        break;
#endif
#ifdef USE_SERIALRX_FPORT
    case SERIALRX_FPORT:
        enabled = fportRxInit(rxConfig, rxRuntimeState);
        break;
#endif
    default:
        enabled = false;
        break;
    }
    return enabled;
}
#endif

#ifdef _USE_HW_CLI
static void cliRx(cli_args_t *args);
#endif

void rxInit(void)
{
    rxRuntimeState.rxProvider = RX_PROVIDER_SERIAL;
    rxRuntimeState.serialrxProvider = SERIALRX_SBUS;
    rxRuntimeState.serialrxProvider = SERIALRX_CRSF;
    rxRuntimeState.rcReadRawFn = nullReadRawRC;
    rxRuntimeState.rcFrameStatusFn = nullFrameStatus;
    rxRuntimeState.rcProcessFrameFn = nullProcessFrame;
    rxRuntimeState.lastRcFrameTimeUs = 0;
    rcSampleIndex = 0;

    for (int i = 0; i < MAX_SUPPORTED_RC_CHANNEL_COUNT; i++) {
        rcData[i] = 1500;
        validRxSignalTimeout[i] = millis() + MAX_INVALID_PULSE_TIME_MS;
    }

    rcData[THROTTLE] = 885;

    // Initialize ARM switch to OFF position when arming via switch is defined
    // TODO - move to rc_mode.c
    // #define MAX_MODE_ACTIVATION_CONDITION_COUNT 20
    // for (int i = 0; i < MAX_MODE_ACTIVATION_CONDITION_COUNT; i++) {
    //     const modeActivationCondition_t *modeActivationCondition = modeActivationConditions(i);
    //     if (modeActivationCondition->modeId == BOXARM && IS_RANGE_USABLE(&modeActivationCondition->range)) {
    //         // ARM switch is defined, determine an OFF value
    //         uint16_t value;
    //         if (modeActivationCondition->range.startStep > 0) {
    //             value = MODE_STEP_TO_CHANNEL_VALUE((modeActivationCondition->range.startStep - 1));
    //         } else {
    //             value = MODE_STEP_TO_CHANNEL_VALUE((modeActivationCondition->range.endStep + 1));
    //         }
    //         // Initialize ARM AUX channel to OFF value
    //         rcData[modeActivationCondition->auxChannelIndex + NON_AUX_CHANNEL_COUNT] = value;
    //     }
    // }

    switch (rxRuntimeState.rxProvider) {
    default:

        break;
#ifdef USE_SERIAL_RX
    case RX_PROVIDER_SERIAL:
        {
            const bool enabled = serialRxInit(&rxRuntimeState);
            if (!enabled) {
                rxRuntimeState.rcReadRawFn = nullReadRawRC;
                rxRuntimeState.rcFrameStatusFn = nullFrameStatus;
            }
        }

        break;
#endif

#ifdef USE_RX_MSP
    case RX_PROVIDER_MSP:
        rxMspInit(rxConfig(), &rxRuntimeState);

        break;
#endif

#ifdef USE_RX_SPI
    case RX_PROVIDER_SPI:
        {
            const bool enabled = rxSpiInit(rxSpiConfig(), &rxRuntimeState);
            if (!enabled) {
                rxRuntimeState.rcReadRawFn = nullReadRawRC;
                rxRuntimeState.rcFrameStatusFn = nullFrameStatus;
            }
        }

        break;
#endif

#if defined(USE_PWM) || defined(USE_PPM)
    case RX_PROVIDER_PPM:
    case RX_PROVIDER_PARALLEL_PWM:
        rxPwmInit(rxConfig(), &rxRuntimeState);

        break;
#endif
    }

//#if defined(USE_ADC)
//    if (featureIsEnabled(FEATURE_RSSI_ADC)) {
//        rssiSource = RSSI_SOURCE_ADC;
//    } else
//#endif
    // if (rxConfig()->rssi_channel > 0) {
    //     rssiSource = RSSI_SOURCE_RX_CHANNEL;
    // }

    // Setup source frame RSSI filtering to take averaged values every FRAME_ERR_RESAMPLE_US
    //pt1FilterInit(&frameErrFilter, pt1FilterGain(GET_FRAME_ERR_LPF_FREQUENCY(rxConfig()->rssi_src_frame_lpf_period), FRAME_ERR_RESAMPLE_US/1000000.0));

    rxChannelCount = MIN(rxConfig()->max_aux_channel + NON_AUX_CHANNEL_COUNT, rxRuntimeState.channelCount);
    
    #ifdef _USE_HW_CLI
        cliAdd("rx", cliRx);
    #endif
}

bool rxIsReceivingSignal(void)
{
    return rxSignalReceived;
}

bool rxAreFlightChannelsValid(void)
{
    return rxFlightChannelsValid;
}

void suspendRxSignal(void)
{
#if defined(USE_PWM) || defined(USE_PPM)
    if (rxRuntimeState.rxProvider == RX_PROVIDER_PARALLEL_PWM || rxRuntimeState.rxProvider == RX_PROVIDER_PPM) {
        suspendRxSignalUntil = micros() + DELAY_1500_MS;  // 1.5s
        skipRxSamples = SKIP_RC_SAMPLES_ON_RESUME;
    }
#endif
    //failsafeOnRxSuspend(DELAY_1500_MS);  // 1.5s
}

void resumeRxSignal(void)
{
#if defined(USE_PWM) || defined(USE_PPM)
    if (rxRuntimeState.rxProvider == RX_PROVIDER_PARALLEL_PWM || rxRuntimeState.rxProvider == RX_PROVIDER_PPM) {
        suspendRxSignalUntil = micros();
        skipRxSamples = SKIP_RC_SAMPLES_ON_RESUME;
    }
#endif
    //failsafeOnRxResume();
}

#ifdef USE_RX_LINK_QUALITY_INFO
#define LINK_QUALITY_SAMPLE_COUNT 16

STATIC_UNIT_TESTED uint16_t updateLinkQualitySamples(uint16_t value)
{
    static uint16_t samples[LINK_QUALITY_SAMPLE_COUNT];
    static uint8_t sampleIndex = 0;
    static uint16_t sum = 0;

    sum += value - samples[sampleIndex];
    samples[sampleIndex] = value;
    sampleIndex = (sampleIndex + 1) % LINK_QUALITY_SAMPLE_COUNT;
    return sum / LINK_QUALITY_SAMPLE_COUNT;
}

void rxSetRfMode(uint8_t rfModeValue)
{
    rfMode = rfModeValue;
}
#endif

#define FRAME_ERR_RESAMPLE_US 100000

static void setLinkQuality(bool validFrame, int32_t currentDeltaTimeUs)
{
    static uint16_t rssiSum = 0;
    static uint16_t rssiCount = 0;
    static int32_t resampleTimeUs = 0;

#ifdef USE_RX_LINK_QUALITY_INFO
    if (linkQualitySource == LQ_SOURCE_NONE) {
        // calculate new sample mean
        linkQuality = updateLinkQualitySamples(validFrame ? LINK_QUALITY_MAX_VALUE : 0);
    }
#endif

    if (rssiSource == RSSI_SOURCE_FRAME_ERRORS) {
        resampleTimeUs += currentDeltaTimeUs;
        rssiSum += validFrame ? RSSI_MAX_VALUE : 0;
        rssiCount++;

        if (resampleTimeUs >= FRAME_ERR_RESAMPLE_US) {
            setRssi(rssiSum / rssiCount, rssiSource);
            rssiSum = 0;
            rssiCount = 0;
            resampleTimeUs -= FRAME_ERR_RESAMPLE_US;
        }
    }
}

void setLinkQualityDirect(uint16_t linkqualityValue)
{
#ifdef USE_RX_LINK_QUALITY_INFO
    linkQuality = linkqualityValue;
#else
    UNUSED(linkqualityValue);
#endif
}

#ifdef USE_RX_LINK_UPLINK_POWER
void rxSetUplinkTxPwrMw(uint16_t uplinkTxPwrMwValue)
{
    uplinkTxPwrMw = uplinkTxPwrMwValue;
}
#endif

bool rxUpdateCheck(uint32_t currentTimeUs, int32_t currentDeltaTimeUs)
{
    UNUSED(currentTimeUs);
    UNUSED(currentDeltaTimeUs);

    return taskUpdateRxMainInProgress() || rxDataProcessingRequired || auxiliaryProcessingRequired;
}

void rxFrameCheck(uint32_t currentTimeUs, int32_t currentDeltaTimeUs)
{
    bool signalReceived = false;
    bool useDataDrivenProcessing = true;
    int32_t needRxSignalMaxDelayUs = DELAY_100_MS;

    //DEBUG_SET(DEBUG_RX_SIGNAL_LOSS, 2, MIN(2000, currentDeltaTimeUs / 100));

    if (taskUpdateRxMainInProgress()) {
        //  no need to check for new data as a packet is being processed already
        return;
    }

    switch (rxRuntimeState.rxProvider) {
    default:

        break;
#if defined(USE_PWM) || defined(USE_PPM)
    case RX_PROVIDER_PPM:
        if (isPPMDataBeingReceived()) {
            signalReceived = true;
            resetPPMDataReceivedState();
        }

        break;
    case RX_PROVIDER_PARALLEL_PWM:
        if (isPWMDataBeingReceived()) {
            signalReceived = true;
            useDataDrivenProcessing = false;
        }

        break;
#endif
    case RX_PROVIDER_SERIAL:
    case RX_PROVIDER_MSP:
    case RX_PROVIDER_SPI:
        {
            const uint8_t frameStatus = rxRuntimeState.rcFrameStatusFn(&rxRuntimeState);
            //DEBUG_SET(DEBUG_RX_SIGNAL_LOSS, 1, (frameStatus & RX_FRAME_FAILSAFE));
            signalReceived = (frameStatus & RX_FRAME_COMPLETE) && !(frameStatus & (RX_FRAME_FAILSAFE | RX_FRAME_DROPPED));
            setLinkQuality(signalReceived, currentDeltaTimeUs);
            auxiliaryProcessingRequired |= (frameStatus & RX_FRAME_PROCESSING_REQUIRED);
        }

        break;
    }

    if (signalReceived) {
        //  true only when a new packet arrives
        needRxSignalBefore = currentTimeUs + needRxSignalMaxDelayUs;
        rxSignalReceived = true; // immediately process packet data
        if (useDataDrivenProcessing) {
            rxDataProcessingRequired = true;
            //  process the new Rx packet when it arrives
        }
    } else {
        //  watch for next packet
        if (cmpTimeUs(currentTimeUs, needRxSignalBefore) > 0) {
            //  initial time to signalReceived failure is 100ms, then we check every 100ms
            rxSignalReceived = false;
            needRxSignalBefore = currentTimeUs + needRxSignalMaxDelayUs;
            //  review and process rcData values every 100ms in case failsafe changed them
            rxDataProcessingRequired = true;
        }
    }

    //DEBUG_SET(DEBUG_RX_SIGNAL_LOSS, 0, rxSignalReceived);
}

#if defined(USE_PWM) || defined(USE_PPM)
static uint16_t calculateChannelMovingAverage(uint8_t chan, uint16_t sample)
{
    static int16_t rcSamples[MAX_SUPPORTED_RX_PARALLEL_PWM_OR_PPM_CHANNEL_COUNT][PPM_AND_PWM_SAMPLE_COUNT];
    static int16_t rcDataMean[MAX_SUPPORTED_RX_PARALLEL_PWM_OR_PPM_CHANNEL_COUNT];
    static bool rxSamplesCollected = false;

    const uint8_t currentSampleIndex = rcSampleIndex % PPM_AND_PWM_SAMPLE_COUNT;

    // update the recent samples and compute the average of them
    rcSamples[chan][currentSampleIndex] = sample;

    // avoid returning an incorrect average which would otherwise occur before enough samples
    if (!rxSamplesCollected) {
        if (rcSampleIndex < PPM_AND_PWM_SAMPLE_COUNT) {
            return sample;
        }
        rxSamplesCollected = true;
    }

    rcDataMean[chan] = 0;
    for (int sampleIndex = 0; sampleIndex < PPM_AND_PWM_SAMPLE_COUNT; sampleIndex++) {
        rcDataMean[chan] += rcSamples[chan][sampleIndex];
    }
    return rcDataMean[chan] / PPM_AND_PWM_SAMPLE_COUNT;
}
#endif

static uint16_t getRxfailValue(uint8_t channel)
{
    //const rxFailsafeChannelConfig_t *channelFailsafeConfig = rxFailsafeChannelConfigs(channel);
    const bool failsafeAuxSwitch = false;//IS_RC_MODE_ACTIVE(BOXFAILSAFE);

    switch (RX_FAILSAFE_MODE_AUTO) { //channelFailsafeConfig->mode
    case RX_FAILSAFE_MODE_AUTO:
        switch (channel) {
        case ROLL:
        case PITCH:
        case YAW:
            return 1500;
        case THROTTLE:
            // if (featureIsEnabled(FEATURE_3D) && !IS_RC_MODE_ACTIVE(BOX3D) && !flight3DConfig()->switched_mode3d) {
            //     return 1500;
            // } else {
                return 885;
            //}
        }

    FALLTHROUGH;
    default:
    case RX_FAILSAFE_MODE_INVALID:
    case RX_FAILSAFE_MODE_HOLD:
        if (failsafeAuxSwitch) {
            return rcRaw[channel]; // current values are allowed through on held channels with switch induced failsafe
        } else {
            return rcData[channel]; // last good value
        }
    case RX_FAILSAFE_MODE_SET:
        return RXFAIL_STEP_TO_CHANNEL_VALUE(CHANNEL_VALUE_TO_RXFAIL_STEP(1000));
    }
}

#define PPM_RCVR_TIMEOUT            0

static float applyRxChannelRangeConfiguraton(float sample)
{
    // Avoid corruption of channel with a value of PPM_RCVR_TIMEOUT
    if (sample == PPM_RCVR_TIMEOUT) {
        return PPM_RCVR_TIMEOUT;
    }

    sample = scaleRangef(sample, 1000, 2000, PWM_RANGE_MIN, PWM_RANGE_MAX);
    // out of range channel values are now constrained after the validity check in detectAndApplySignalLossBehaviour()
    return sample;
}

static void readRxChannelsApplyRanges(void)
{
    for (int channel = 0; channel < rxChannelCount; channel++) {

        const uint8_t rawChannel = channel < RX_MAPPABLE_CHANNEL_COUNT ? rcmap[channel] : channel;

        // sample the channel
        float sample;
#if defined(USE_RX_MSP_OVERRIDE)
        if (rxConfig()->msp_override_channels_mask) {
            sample = rxMspOverrideReadRawRc(&rxRuntimeState, rxConfig(), rawChannel);
        } else
#endif
        {
            sample = rxRuntimeState.rcReadRawFn(&rxRuntimeState, rawChannel);
        }

        // apply the rx calibration
        if (channel < NON_AUX_CHANNEL_COUNT) {
            sample = applyRxChannelRangeConfiguraton(sample);
        }

        rcRaw[channel] = sample;
    }
}

void detectAndApplySignalLossBehaviour(void)
{
    const uint32_t currentTimeMs = millis();
    const bool failsafeAuxSwitch = false; //IS_RC_MODE_ACTIVE(BOXFAILSAFE);
    rxFlightChannelsValid = rxSignalReceived && !failsafeAuxSwitch;
    //  set rxFlightChannelsValid false when a packet is bad or we use a failsafe switch

    for (int channel = 0; channel < rxChannelCount; channel++) {
        float sample = rcRaw[channel]; // sample has latest RC value, rcData has last 'accepted valid' value
        const bool thisChannelValid = rxFlightChannelsValid && isPulseValid(sample);
        // if the whole packet is bad, consider all channels bad

        if (thisChannelValid) {
            //  reset the invalid pulse period timer for every good channel
            validRxSignalTimeout[channel] = currentTimeMs + MAX_INVALID_PULSE_TIME_MS;
        }

       if (false) { //ARMING_FLAG(ARMED) && failsafeIsActive()
            // while in failsafe Stage 2, whether Rx loss or switch induced, pass valid incoming flight channel values
            // this allows GPS Rescue to detect the 30% requirement for termination
            if (channel < NON_AUX_CHANNEL_COUNT) {
                if (!thisChannelValid) {
                    if (channel == THROTTLE ) {
                        sample = 1150; // stage 2 failsafe throttle value
                    } else {
                        sample = 1500;
                    }
                }
            } else {
                //  During Stage 2, set aux channels as per Stage 1 configuration
                sample = getRxfailValue(channel);
            }
        } else {
            if (failsafeAuxSwitch) {
                sample = getRxfailValue(channel);
                //  set channels to Stage 1 values immediately failsafe switch is activated
            } else if (!thisChannelValid) {
                if (cmp32(currentTimeMs, validRxSignalTimeout[channel]) < 0) {
                    // first 300ms of Stage 1 failsafe
                    sample = rcData[channel];
                    //  HOLD last valid value on bad channel/s for MAX_INVALID_PULSE_TIME_MS (300ms)
                } else {
                    // remaining Stage 1 failsafe period after 300ms
                    if (channel < NON_AUX_CHANNEL_COUNT) {
                        rxFlightChannelsValid = false;
                        //  declare signal lost after 300ms of any one bad flight channel
                    }
                    sample = getRxfailValue(channel);
                    // set channels that are invalid for more than 300ms to Stage 1 values
                }
            }
        }

        sample = constrainf(sample, PWM_PULSE_MIN, PWM_PULSE_MAX);

#if defined(USE_PWM) || defined(USE_PPM)
        if (rxRuntimeState.rxProvider == RX_PROVIDER_PARALLEL_PWM || rxRuntimeState.rxProvider == RX_PROVIDER_PPM) {
            //  smooth output for PWM and PPM using moving average
            rcData[channel] = calculateChannelMovingAverage(channel, sample);
        } else
#endif

        {
            //  set rcData to either validated incoming values, or failsafe-modified values
            rcData[channel] = sample;
        }
    }

    if (rxFlightChannelsValid) {
        //failsafeOnValidDataReceived();
        //  --> start the timer to exit stage 2 failsafe
    } else {
        //failsafeOnValidDataFailed();
        //  -> start timer to enter stage2 failsafe
    }

    //DEBUG_SET(DEBUG_RX_SIGNAL_LOSS, 3, rcData[THROTTLE]);
}

bool calculateRxChannelsAndUpdateFailsafe(uint32_t currentTimeUs)
{
    if (auxiliaryProcessingRequired) {
        auxiliaryProcessingRequired = !rxRuntimeState.rcProcessFrameFn(&rxRuntimeState);
    }

    if (!rxDataProcessingRequired) {
        return false;
    }

    rxDataProcessingRequired = false;

    // only proceed when no more samples to skip and suspend period is over
    if (skipRxSamples || currentTimeUs <= suspendRxSignalUntil) {
        if (currentTimeUs > suspendRxSignalUntil) {
            skipRxSamples--;
        }

        return true;
    }

    while(uartAvailable(_DEF_UART2)){
        sbusDataReceive(uartRead(_DEF_UART2), rxRuntimeState.frameData);
    }
        while(uartAvailable(_DEF_UART2)){
        crsfDataReceive(uartRead(_DEF_UART2), rxRuntimeState.frameData);
    }

    readRxChannelsApplyRanges();            // returns rcRaw
    detectAndApplySignalLossBehaviour();    // returns rcData

    rcSampleIndex++;

    return true;
}

void parseRcChannels(const char *input, rxConfig_t *rxConfig)
{
    for (const char *c = input; *c; c++) {
        const char *s = strchr(rcChannelLetters, *c);
        if (s && (s < rcChannelLetters + RX_MAPPABLE_CHANNEL_COUNT)) {
            rxConfig->rcmap[s - rcChannelLetters] = c - input;
        }
    }
}

void setRssiDirect(uint16_t newRssi, rssiSource_e source)
{
    if (source != rssiSource) {
        return;
    }

    rssi = newRssi;
}

#define RSSI_SAMPLE_COUNT 16

static uint16_t updateRssiSamples(uint16_t value)
{
    static uint16_t samples[RSSI_SAMPLE_COUNT];
    static uint8_t sampleIndex = 0;
    static unsigned sum = 0;

    sum += value - samples[sampleIndex];
    samples[sampleIndex] = value;
    sampleIndex = (sampleIndex + 1) % RSSI_SAMPLE_COUNT;
    return sum / RSSI_SAMPLE_COUNT;
}

void setRssi(uint16_t rssiValue, rssiSource_e source)
{
    if (source != rssiSource) {
        return;
    }

    // Filter RSSI value
    if (source == RSSI_SOURCE_FRAME_ERRORS) {
        //rssi = pt1FilterApply(&frameErrFilter, rssiValue);
    } else {
        // calculate new sample mean
        rssi = updateRssiSamples(rssiValue);
    }
}

void setRssiMsp(uint8_t newMspRssi)
{
    if (rssiSource == RSSI_SOURCE_NONE) {
        rssiSource = RSSI_SOURCE_MSP;
    }

    if (rssiSource == RSSI_SOURCE_MSP) {
        rssi = ((uint16_t)newMspRssi) << 2;
        lastMspRssiUpdateUs = micros();
    }
}

static void updateRSSIPWM(void)
{
    // Read value of AUX channel as rssi
    int16_t pwmRssi = rcData[10 - 1]; //.rssi_channel = 0

    // Range of rawPwmRssi is [1000;2000]. rssi should be in [0;1023];
    setRssiDirect(scaleRange(constrain(pwmRssi, PWM_RANGE_MIN, PWM_RANGE_MAX), PWM_RANGE_MIN, PWM_RANGE_MAX, 0, RSSI_MAX_VALUE), RSSI_SOURCE_RX_CHANNEL);
}

static void updateRSSIADC(uint32_t currentTimeUs)
{
#ifndef USE_ADC
    UNUSED(currentTimeUs);
#else
//    static uint32_t rssiUpdateAt = 0;
//
//    if ((int32_t)(currentTimeUs - rssiUpdateAt) < 0) {
//        return;
//    }
//    rssiUpdateAt = currentTimeUs + DELAY_20_MS;
//
//    const uint16_t adcRssiSample = adcGetChannel(ADC_RSSI);
//    uint16_t rssiValue = adcRssiSample / RSSI_ADC_DIVISOR;
//
//    setRssi(rssiValue, RSSI_SOURCE_ADC);
#endif
}

void updateRSSI(uint32_t currentTimeUs)
{
    switch (rssiSource) {
    case RSSI_SOURCE_RX_CHANNEL:
        updateRSSIPWM();
        break;
    case RSSI_SOURCE_ADC:
        updateRSSIADC(currentTimeUs);
        break;
    case RSSI_SOURCE_MSP:
        if (cmpTimeUs(micros(), lastMspRssiUpdateUs) > DELAY_1500_MS) {  // 1.5s
            rssi = 0;
        }
        break;
    default:
        break;
    }
}

uint16_t getRssi(void)
{
    uint16_t rssiValue = rssi;

    // RSSI_Invert option
    if (0) { //.rssi_invert = 0
        rssiValue = RSSI_MAX_VALUE - rssiValue;
    }

    return 100 / 100.0f * rssiValue + 0 * RSSI_OFFSET_SCALING; //.rssi_scale = 100, .rssi_offset = 0
}

uint8_t getRssiPercent(void)
{
    return scaleRange(getRssi(), 0, RSSI_MAX_VALUE, 0, 100);
}

int16_t getRssiDbm(void)
{
    return rssiDbm;
}

#define RSSI_SAMPLE_COUNT_DBM 16

static int16_t updateRssiDbmSamples(int16_t value)
{
    static int16_t samplesdbm[RSSI_SAMPLE_COUNT_DBM];
    static uint8_t sampledbmIndex = 0;
    static int sumdbm = 0;

    sumdbm += value - samplesdbm[sampledbmIndex];
    samplesdbm[sampledbmIndex] = value;
    sampledbmIndex = (sampledbmIndex + 1) % RSSI_SAMPLE_COUNT_DBM;
    return sumdbm / RSSI_SAMPLE_COUNT_DBM;
}

void setRssiDbm(int16_t rssiDbmValue, rssiSource_e source)
{
    if (source != rssiSource) {
        return;
    }

    rssiDbm = updateRssiDbmSamples(rssiDbmValue);
}

void setRssiDbmDirect(int16_t newRssiDbm, rssiSource_e source)
{
    if (source != rssiSource) {
        return;
    }

    rssiDbm = newRssiDbm;
}

#ifdef USE_RX_LINK_QUALITY_INFO
uint16_t rxGetLinkQuality(void)
{
    return linkQuality;
}

uint8_t rxGetRfMode(void)
{
    return rfMode;
}

uint16_t rxGetLinkQualityPercent(void)
{
    return (linkQualitySource == LQ_SOURCE_NONE) ? scaleRange(linkQuality, 0, LINK_QUALITY_MAX_VALUE, 0, 100) : linkQuality;
}
#endif

#ifdef USE_RX_LINK_UPLINK_POWER
uint16_t rxGetUplinkTxPwrMw(void)
{
    return uplinkTxPwrMw;
}
#endif

uint16_t rxGetRefreshRate(void)
{
    return rxRuntimeState.rxRefreshRate;
}

bool isRssiConfigured(void)
{
    return rssiSource != RSSI_SOURCE_NONE;
}

int32_t rxGetFrameDelta(int32_t *frameAgeUs)
{
    static uint32_t previousFrameTimeUs = 0;
    static int32_t frameTimeDeltaUs = 0;

    if (rxRuntimeState.rcFrameTimeUsFn) {
        const uint32_t frameTimeUs = rxRuntimeState.rcFrameTimeUsFn();

        *frameAgeUs = cmpTimeUs(micros(), frameTimeUs);

        const int32_t deltaUs = cmpTimeUs(frameTimeUs, previousFrameTimeUs);
        if (deltaUs) {
            frameTimeDeltaUs = deltaUs;
            previousFrameTimeUs = frameTimeUs;
        }
    }

    return frameTimeDeltaUs;
}

uint32_t rxFrameTimeUs(void)
{
    return rxRuntimeState.lastRcFrameTimeUs;
}



/*
 * processRx called from taskUpdateRxMain
 */
bool processRx(uint32_t currentTimeUs)
{
    if (!calculateRxChannelsAndUpdateFailsafe(currentTimeUs)) {
        return false;
    }

    //updateRcRefreshRate(currentTimeUs);

    // in 3D mode, we need to be able to disarm by switch at any time
    // if (featureIsEnabled(FEATURE_3D)) {
    //     if (!IS_RC_MODE_ACTIVE(BOXARM))
    //         disarm(DISARM_REASON_SWITCH);
    // }

    updateRSSI(currentTimeUs);

    // if (currentTimeUs > FAILSAFE_POWER_ON_DELAY_US && !failsafeIsMonitoring()) {
    //     failsafeStartMonitoring();
    // }

    // const throttleStatus_e throttleStatus = calculateThrottleStatus();
    // const uint8_t throttlePercent = calculateThrottlePercentAbs();

    // const bool launchControlActive = isLaunchControlActive();

    // if (airmodeIsEnabled() && ARMING_FLAG(ARMED) && !launchControlActive) {
    //     if (throttlePercent >= rxConfig()->airModeActivateThreshold) {
    //         airmodeIsActivated = true; // Prevent iterm from being reset
    //     }
    // } else {
    //     airmodeIsActivated = false;
    // }

    /* In airmode iterm should be prevented to grow when Low thottle and Roll + Pitch Centered.
     This is needed to prevent iterm winding on the ground, but keep full stabilisation on 0 throttle while in air */
    // if (throttleStatus == THROTTLE_LOW && !airmodeIsActivated && !launchControlActive) {
    //     pidSetItermReset(true);
    //     if (currentPidProfile->pidAtMinThrottle)
    //         pidStabilisationState(PID_STABILISATION_ON);
    //     else
    //         pidStabilisationState(PID_STABILISATION_OFF);
    // } else {
    //     pidSetItermReset(false);
    //     pidStabilisationState(PID_STABILISATION_ON);
    // }

#ifdef USE_RUNAWAY_TAKEOFF
    // If runaway_takeoff_prevention is enabled, accumulate the amount of time that throttle
    // is above runaway_takeoff_deactivate_throttle with the any of the R/P/Y sticks deflected
    // to at least runaway_takeoff_stick_percent percent while the pidSum on all axis is kept low.
    // Once the amount of accumulated time exceeds runaway_takeoff_deactivate_delay then disable
    // prevention for the remainder of the battery.

    if (ARMING_FLAG(ARMED)
        && pidConfig()->runaway_takeoff_prevention
        && !runawayTakeoffCheckDisabled
        && !flipOverAfterCrashActive
        && !runawayTakeoffTemporarilyDisabled
        && !isFixedWing()) {

        // Determine if we're in "flight"
        //   - motors running
        //   - throttle over runaway_takeoff_deactivate_throttle_percent
        //   - sticks are active and have deflection greater than runaway_takeoff_deactivate_stick_percent
        //   - pidSum on all axis is less then runaway_takeoff_deactivate_pidlimit
        bool inStableFlight = false;
        if (!featureIsEnabled(FEATURE_MOTOR_STOP) || airmodeIsEnabled() || (throttleStatus != THROTTLE_LOW)) { // are motors running?
            const uint8_t lowThrottleLimit = pidConfig()->runaway_takeoff_deactivate_throttle;
            const uint8_t midThrottleLimit = constrain(lowThrottleLimit * 2, lowThrottleLimit * 2, RUNAWAY_TAKEOFF_HIGH_THROTTLE_PERCENT);
            if ((((throttlePercent >= lowThrottleLimit) && areSticksActive(RUNAWAY_TAKEOFF_DEACTIVATE_STICK_PERCENT)) || (throttlePercent >= midThrottleLimit))
                && (fabsf(pidData[FD_PITCH].Sum) < RUNAWAY_TAKEOFF_DEACTIVATE_PIDSUM_LIMIT)
                && (fabsf(pidData[FD_ROLL].Sum) < RUNAWAY_TAKEOFF_DEACTIVATE_PIDSUM_LIMIT)
                && (fabsf(pidData[FD_YAW].Sum) < RUNAWAY_TAKEOFF_DEACTIVATE_PIDSUM_LIMIT)) {

                inStableFlight = true;
                if (runawayTakeoffDeactivateUs == 0) {
                    runawayTakeoffDeactivateUs = currentTimeUs;
                }
            }
        }

        // If we're in flight, then accumulate the time and deactivate once it exceeds runaway_takeoff_deactivate_delay milliseconds
        if (inStableFlight) {
            if (runawayTakeoffDeactivateUs == 0) {
                runawayTakeoffDeactivateUs = currentTimeUs;
            }
            uint16_t deactivateDelay = pidConfig()->runaway_takeoff_deactivate_delay;
            // at high throttle levels reduce deactivation delay by 50%
            if (throttlePercent >= RUNAWAY_TAKEOFF_HIGH_THROTTLE_PERCENT) {
                deactivateDelay = deactivateDelay / 2;
            }
            if ((cmpTimeUs(currentTimeUs, runawayTakeoffDeactivateUs) + runawayTakeoffAccumulatedUs) > deactivateDelay * 1000) {
                runawayTakeoffCheckDisabled = true;
            }

        } else {
            if (runawayTakeoffDeactivateUs != 0) {
                runawayTakeoffAccumulatedUs += cmpTimeUs(currentTimeUs, runawayTakeoffDeactivateUs);
            }
            runawayTakeoffDeactivateUs = 0;
        }
        if (runawayTakeoffDeactivateUs == 0) {
            DEBUG_SET(DEBUG_RUNAWAY_TAKEOFF, DEBUG_RUNAWAY_TAKEOFF_DEACTIVATING_DELAY, DEBUG_RUNAWAY_TAKEOFF_FALSE);
            DEBUG_SET(DEBUG_RUNAWAY_TAKEOFF, DEBUG_RUNAWAY_TAKEOFF_DEACTIVATING_TIME, runawayTakeoffAccumulatedUs / 1000);
        } else {
            DEBUG_SET(DEBUG_RUNAWAY_TAKEOFF, DEBUG_RUNAWAY_TAKEOFF_DEACTIVATING_DELAY, DEBUG_RUNAWAY_TAKEOFF_TRUE);
            DEBUG_SET(DEBUG_RUNAWAY_TAKEOFF, DEBUG_RUNAWAY_TAKEOFF_DEACTIVATING_TIME, (cmpTimeUs(currentTimeUs, runawayTakeoffDeactivateUs) + runawayTakeoffAccumulatedUs) / 1000);
        }
    } else {
        DEBUG_SET(DEBUG_RUNAWAY_TAKEOFF, DEBUG_RUNAWAY_TAKEOFF_DEACTIVATING_DELAY, DEBUG_RUNAWAY_TAKEOFF_FALSE);
        DEBUG_SET(DEBUG_RUNAWAY_TAKEOFF, DEBUG_RUNAWAY_TAKEOFF_DEACTIVATING_TIME, DEBUG_RUNAWAY_TAKEOFF_FALSE);
    }
#endif

// #ifdef USE_LAUNCH_CONTROL
//     if (ARMING_FLAG(ARMED)) {
//         if (launchControlActive && (throttlePercent > currentPidProfile->launchControlThrottlePercent)) {
//             // throttle limit trigger reached, launch triggered
//             // reset the iterms as they may be at high values from holding the launch position
//             launchControlState = LAUNCH_CONTROL_TRIGGERED;
//             pidResetIterm();
//         }
//     } else {
//         if (launchControlState == LAUNCH_CONTROL_TRIGGERED) {
//             // If trigger mode is MULTIPLE then reset the state when disarmed
//             // and the mode switch is turned off.
//             // For trigger mode SINGLE we never reset the state and only a single
//             // launch is allowed until a reboot.
//             if (currentPidProfile->launchControlAllowTriggerReset && !IS_RC_MODE_ACTIVE(BOXLAUNCHCONTROL)) {
//                 launchControlState = LAUNCH_CONTROL_DISABLED;
//             }
//         } else {
//             launchControlState = LAUNCH_CONTROL_DISABLED;
//         }
//     }
// #endif

    return true;
}

void processRxModes(uint32_t currentTimeUs)
{
    //static bool armedBeeperOn = false;
#ifdef USE_TELEMETRY
    static bool sharedPortTelemetryEnabled = false;
#endif
    //const throttleStatus_e throttleStatus = calculateThrottleStatus();

    // When armed and motors aren't spinning, do beeps and then disarm
    // board after delay so users without buzzer won't lose fingers.
    // mixTable constrains motor commands, so checking  throttleStatus is enough
    // const timeUs_t autoDisarmDelayUs = armingConfig()->auto_disarm_delay * 1e6;
    // if (ARMING_FLAG(ARMED)
    //     && featureIsEnabled(FEATURE_MOTOR_STOP)
    //     && !isFixedWing()
    //     && !featureIsEnabled(FEATURE_3D)
    //     && !airmodeIsEnabled()
    //     && !FLIGHT_MODE(GPS_RESCUE_MODE)  // disable auto-disarm when GPS Rescue is active
    // ) {
    //     if (isUsingSticksForArming()) {
    //         if (throttleStatus == THROTTLE_LOW) {
    //             if ((autoDisarmDelayUs > 0) && (currentTimeUs > disarmAt)) {
    //                 // auto-disarm configured and delay is over
    //                 disarm(DISARM_REASON_THROTTLE_TIMEOUT);
    //                 armedBeeperOn = false;
    //             } else {
    //                 // still armed; do warning beeps while armed
    //                 beeper(BEEPER_ARMED);
    //                 armedBeeperOn = true;
    //             }
    //         } else {
    //             // throttle is not low - extend disarm time
    //             disarmAt = currentTimeUs + autoDisarmDelayUs;

    //             if (armedBeeperOn) {
    //                 beeperSilence();
    //                 armedBeeperOn = false;
    //             }
    //         }
    //     } else {
    //         // arming is via AUX switch; beep while throttle low
    //         if (throttleStatus == THROTTLE_LOW) {
    //             beeper(BEEPER_ARMED);
    //             armedBeeperOn = true;
    //         } else if (armedBeeperOn) {
    //             beeperSilence();
    //             armedBeeperOn = false;
    //         }
    //     }
    // } else {
    //     disarmAt = currentTimeUs + autoDisarmDelayUs;  // extend auto-disarm timer
    // }

    // if (!(IS_RC_MODE_ACTIVE(BOXPARALYZE) && !ARMING_FLAG(ARMED))
#ifdef USE_CMS
        && !cmsInMenu
#endif
    //    ) {
    //    processRcStickPositions();
    //}

    // if (featureIsEnabled(FEATURE_INFLIGHT_ACC_CAL)) {
    //     updateInflightCalibrationState();
    // }

    //updateActivatedModes();

#ifdef USE_DSHOT
    /* Enable beep warning when the crash flip mode is active */
    if (flipOverAfterCrashActive) {
        beeper(BEEPER_CRASH_FLIP_MODE);
    }
#endif

    // if (!cliMode && !(IS_RC_MODE_ACTIVE(BOXPARALYZE) && !ARMING_FLAG(ARMED))) {
    //     processRcAdjustments(currentControlRateProfile);
    // }

    // bool canUseHorizonMode = true;

    // if ((IS_RC_MODE_ACTIVE(BOXANGLE) || failsafeIsActive()) && (sensors(SENSOR_ACC))) {
    //     // bumpless transfer to Level mode
    //     canUseHorizonMode = false;

    //     if (!FLIGHT_MODE(ANGLE_MODE)) {
    //         ENABLE_FLIGHT_MODE(ANGLE_MODE);
    //     }
    // } else {
    //     DISABLE_FLIGHT_MODE(ANGLE_MODE); // failsafe support
    // }

    // if (IS_RC_MODE_ACTIVE(BOXHORIZON) && canUseHorizonMode) {

    //     DISABLE_FLIGHT_MODE(ANGLE_MODE);

    //     if (!FLIGHT_MODE(HORIZON_MODE)) {
    //         ENABLE_FLIGHT_MODE(HORIZON_MODE);
    //     }
    // } else {
    //     DISABLE_FLIGHT_MODE(HORIZON_MODE);
    // }

#ifdef USE_GPS_RESCUE
    if (ARMING_FLAG(ARMED) && (IS_RC_MODE_ACTIVE(BOXGPSRESCUE) || (failsafeIsActive() && failsafeConfig()->failsafe_procedure == FAILSAFE_PROCEDURE_GPS_RESCUE))) {
        if (!FLIGHT_MODE(GPS_RESCUE_MODE)) {
            ENABLE_FLIGHT_MODE(GPS_RESCUE_MODE);
        }
    } else {
        DISABLE_FLIGHT_MODE(GPS_RESCUE_MODE);
    }
#endif

    // if (FLIGHT_MODE(ANGLE_MODE) || FLIGHT_MODE(HORIZON_MODE)) {
    //     LED1_ON;
    //     // increase frequency of attitude task to reduce drift when in angle or horizon mode
    //     rescheduleTask(TASK_ATTITUDE, TASK_PERIOD_HZ(500));
    // } else {
    //     LED1_OFF;
    //     rescheduleTask(TASK_ATTITUDE, TASK_PERIOD_HZ(100));
    // }

    // if (!IS_RC_MODE_ACTIVE(BOXPREARM) && ARMING_FLAG(WAS_ARMED_WITH_PREARM)) {
    //     DISABLE_ARMING_FLAG(WAS_ARMED_WITH_PREARM);
    // }

// #if defined(USE_ACC) || defined(USE_MAG)
//     if (sensors(SENSOR_ACC) || sensors(SENSOR_MAG)) {
// #if defined(USE_GPS) || defined(USE_MAG)
//         if (IS_RC_MODE_ACTIVE(BOXMAG)) {
//             if (!FLIGHT_MODE(MAG_MODE)) {
//                 ENABLE_FLIGHT_MODE(MAG_MODE);
//                 magHold = DECIDEGREES_TO_DEGREES(attitude.values.yaw);
//             }
//         } else {
//             DISABLE_FLIGHT_MODE(MAG_MODE);
//         }
// #endif
//         if (IS_RC_MODE_ACTIVE(BOXHEADFREE) && !FLIGHT_MODE(GPS_RESCUE_MODE)) {
//             if (!FLIGHT_MODE(HEADFREE_MODE)) {
//                 ENABLE_FLIGHT_MODE(HEADFREE_MODE);
//             }
//         } else {
//             DISABLE_FLIGHT_MODE(HEADFREE_MODE);
//         }
//         if (IS_RC_MODE_ACTIVE(BOXHEADADJ) && !FLIGHT_MODE(GPS_RESCUE_MODE)) {
//             if (imuQuaternionHeadfreeOffsetSet()) {
//                beeper(BEEPER_RX_SET);
//             }
//         }
//     }
// #endif

    // if (IS_RC_MODE_ACTIVE(BOXPASSTHRU)) {
    //     ENABLE_FLIGHT_MODE(PASSTHRU_MODE);
    // } else {
    //     DISABLE_FLIGHT_MODE(PASSTHRU_MODE);
    // }

    // if (mixerConfig()->mixerMode == MIXER_FLYING_WING || mixerConfig()->mixerMode == MIXER_AIRPLANE) {
    //     DISABLE_FLIGHT_MODE(HEADFREE_MODE);
    // }

#ifdef USE_TELEMETRY
    if (featureIsEnabled(FEATURE_TELEMETRY)) {
        bool enableSharedPortTelemetry = (!isModeActivationConditionPresent(BOXTELEMETRY) && ARMING_FLAG(ARMED)) || (isModeActivationConditionPresent(BOXTELEMETRY) && IS_RC_MODE_ACTIVE(BOXTELEMETRY));
        if (enableSharedPortTelemetry && !sharedPortTelemetryEnabled) {
            mspSerialReleaseSharedTelemetryPorts();
            telemetryCheckState();

            sharedPortTelemetryEnabled = true;
        } else if (!enableSharedPortTelemetry && sharedPortTelemetryEnabled) {
            // the telemetry state must be checked immediately so that shared serial ports are released.
            telemetryCheckState();
            mspSerialAllocatePorts();

            sharedPortTelemetryEnabled = false;
        }
    }
#endif

#ifdef USE_VTX_CONTROL
    vtxUpdateActivatedChannel();

    if (canUpdateVTX()) {
        handleVTXControlButton();
    }
#endif

#ifdef USE_ACRO_TRAINER
    pidSetAcroTrainerState(IS_RC_MODE_ACTIVE(BOXACROTRAINER) && sensors(SENSOR_ACC));
#endif // USE_ACRO_TRAINER

#ifdef USE_RC_SMOOTHING_FILTER
    if (ARMING_FLAG(ARMED) && !rcSmoothingInitializationComplete()) {
        beeper(BEEPER_RC_SMOOTHING_INIT_FAIL);
    }
#endif

    //pidSetAntiGravityState(IS_RC_MODE_ACTIVE(BOXANTIGRAVITY) || featureIsEnabled(FEATURE_ANTI_GRAVITY));
}

#define THROTTLE_LOOKUP_LENGTH 12
static int16_t lookupThrottleRC[THROTTLE_LOOKUP_LENGTH];    // lookup table for expo & mid THROTTLE

static int16_t rcLookupThrottle(int32_t tmp)
{
    const int32_t tmp2 = tmp / 100;
    // [0;1000] -> expo -> [MINTHROTTLE;MAXTHROTTLE]
    return lookupThrottleRC[tmp2] + (tmp - tmp2 * 100) * (lookupThrottleRC[tmp2 + 1] - lookupThrottleRC[tmp2]) / 100;
}

void updateRcCommands(void)
{
    isRxDataNew = true;

    for (int axis = 0; axis < 3; axis++) {
        // non coupled PID reduction scaler used in PID controller 1 and PID controller 2.

        float tmp = MIN(ABS(rcData[axis] - rxConfig()->midrc), 500);
        if (axis == ROLL || axis == PITCH) {
            if (tmp > 5) {
                tmp -= 5;
            } else {
                tmp = 0;
            }
            rcCommand[axis] = tmp;
        } else {
            if (tmp > 5) {
                tmp -= 5;
            } else {
                tmp = 0;
            }
            rcCommand[axis] = tmp * -GET_DIRECTION(false);
        }
        if (rcData[axis] < rxConfig()->midrc) {
            rcCommand[axis] = -rcCommand[axis];
        }
    }

    int32_t tmp;
    // if (featureIsEnabled(FEATURE_3D)) {
    //     tmp = constrain(rcData[THROTTLE], PWM_RANGE_MIN, PWM_RANGE_MAX);
    //     tmp = (uint32_t)(tmp - PWM_RANGE_MIN);
    // } else
    {
        tmp = constrain(rcData[THROTTLE], rxConfig()->mincheck, PWM_RANGE_MAX);
        tmp = (uint32_t)(tmp - rxConfig()->mincheck) * PWM_RANGE_MIN / (PWM_RANGE_MAX - rxConfig()->mincheck);
    }

    // if (getLowVoltageCutoff()->enabled) {
    //     tmp = tmp * getLowVoltageCutoff()->percentage / 100;
    // }

    rcCommand[THROTTLE] = rcLookupThrottle(tmp);

    // if (featureIsEnabled(FEATURE_3D) && !failsafeIsActive()) {
    //     if (!flight3DConfig()->switched_mode3d) {
    //         if (IS_RC_MODE_ACTIVE(BOX3D)) {
    //             fix12_t throttleScaler = qConstruct(rcCommand[THROTTLE] - 1000, 1000);
    //             rcCommand[THROTTLE] = rxConfig()->midrc + qMultiply(throttleScaler, PWM_RANGE_MAX - rxConfig()->midrc);
    //         }
    //     } else {
    //         if (IS_RC_MODE_ACTIVE(BOX3D)) {
    //             reverseMotors = true;
    //             fix12_t throttleScaler = qConstruct(rcCommand[THROTTLE] - 1000, 1000);
    //             rcCommand[THROTTLE] = rxConfig()->midrc + qMultiply(throttleScaler, PWM_RANGE_MIN - rxConfig()->midrc);
    //         } else {
    //             reverseMotors = false;
    //             fix12_t throttleScaler = qConstruct(rcCommand[THROTTLE] - 1000, 1000);
    //             rcCommand[THROTTLE] = rxConfig()->midrc + qMultiply(throttleScaler, PWM_RANGE_MAX - rxConfig()->midrc);
    //         }
    //     }
    // }
    // if (FLIGHT_MODE(HEADFREE_MODE)) {
    //     static t_fp_vector_def  rcCommandBuff;

    //     rcCommandBuff.X = rcCommand[ROLL];
    //     rcCommandBuff.Y = rcCommand[PITCH];
    //     if ((!FLIGHT_MODE(ANGLE_MODE) && (!FLIGHT_MODE(HORIZON_MODE)) && (!FLIGHT_MODE(GPS_RESCUE_MODE)))) {
    //         rcCommandBuff.Z = rcCommand[YAW];
    //     } else {
    //         rcCommandBuff.Z = 0;
    //     }
    //     imuQuaternionHeadfreeTransformVectorEarthToBody(&rcCommandBuff);
    //     rcCommand[ROLL] = rcCommandBuff.X;
    //     rcCommand[PITCH] = rcCommandBuff.Y;
    //     if ((!FLIGHT_MODE(ANGLE_MODE)&&(!FLIGHT_MODE(HORIZON_MODE)) && (!FLIGHT_MODE(GPS_RESCUE_MODE)))) {
    //         rcCommand[YAW] = rcCommandBuff.Z;
    //     }
    // }
}


#ifdef _USE_HW_CLI
void cliRx(cli_args_t *args)
{
  bool ret = false;

if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    uint32_t pre_time;
 	pre_time = millis();
    while(cliKeepLoop())
    {
        scheduler();
        if (millis()-pre_time >= 1000)
    	{
     		pre_time = millis();
            cliPrintf("rx: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n\r", rcRaw[0], rcRaw[1], rcRaw[2], rcRaw[3], rcRaw[4],
            rcRaw[5], rcRaw[6], rcRaw[7], rcRaw[8], rcRaw[9]);
    	}
    }
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("Rx show \n\r");
  }
}
#endif
