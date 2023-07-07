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

#include "utils.h"

#include "adc_impl.h"

#include "adc.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

//#define DEBUG_ADC_CHANNELS

adcOperatingConfig_t adcOperatingConfig[ADC_CHANNEL_COUNT];

volatile uint16_t adcValues[ADC_CHANNEL_COUNT];

// void adcInitDevice(ADC_TypeDef *adcdev, int channelCount)
// {
//     ADC_InitTypeDef ADC_InitStructure;

//     ADC_StructInit(&ADC_InitStructure);

//     ADC_InitStructure.ADC_ContinuousConvMode       = ENABLE;
//     ADC_InitStructure.ADC_Resolution               = ADC_Resolution_12b;
//     ADC_InitStructure.ADC_ExternalTrigConv         = ADC_ExternalTrigConv_T1_CC1;
//     ADC_InitStructure.ADC_ExternalTrigConvEdge     = ADC_ExternalTrigConvEdge_None;
//     ADC_InitStructure.ADC_DataAlign                = ADC_DataAlign_Right;
//     ADC_InitStructure.ADC_NbrOfConversion          = channelCount;

//     // Multiple injected channel seems to require scan conversion mode to be
//     // enabled even if main (non-injected) channel count is 1.
// #ifdef USE_ADC_INTERNAL
//     ADC_InitStructure.ADC_ScanConvMode             = ENABLE;
// #else
//     ADC_InitStructure.ADC_ScanConvMode             = channelCount > 1 ? ENABLE : DISABLE; // 1=scan more that one channel in group
// #endif
//     ADC_Init(adcdev, &ADC_InitStructure);
// }

#ifdef USE_ADC_INTERNAL
void adcInitInternalInjected(const adcConfig_t *config)
{
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_InjectedDiscModeCmd(ADC1, DISABLE);
    ADC_InjectedSequencerLengthConfig(ADC1, 2);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_480Cycles);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_TempSensor, 2, ADC_SampleTime_480Cycles);

    adcVREFINTCAL = config->vrefIntCalibration ? config->vrefIntCalibration : *(uint16_t *)VREFINT_CAL_ADDR;
    adcTSCAL1 = config->tempSensorCalibration1 ? config->tempSensorCalibration1 : *(uint16_t *)TS_CAL1_ADDR;
    adcTSCAL2 = config->tempSensorCalibration2 ? config->tempSensorCalibration2 : *(uint16_t *)TS_CAL2_ADDR;

    adcTSSlopeK = (110 - 30) * 1000 / (adcTSCAL2 - adcTSCAL1);
}

// Note on sampling time for temperature sensor and vrefint:
// Both sources have minimum sample time of 10us.
// With prescaler = 8:
// 168MHz : fAPB2 = 84MHz, fADC = 10.5MHz, tcycle = 0.090us, 10us = 105cycle < 144cycle
// 240MHz : fAPB2 = 120MHz, fADC = 15.0MHz, tcycle = 0.067usk 10us = 150cycle < 480cycle
//
// 480cycles@15.0MHz = 32us

static bool adcInternalConversionInProgress = false;

bool adcInternalIsBusy(void)
{
    if (adcInternalConversionInProgress) {
        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) != RESET) {
            adcInternalConversionInProgress = false;
        }
    }

    return adcInternalConversionInProgress;
}

void adcInternalStartConversion(void)
{
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
    ADC_SoftwareStartInjectedConv(ADC1);

    adcInternalConversionInProgress = true;
}

uint16_t adcInternalReadVrefint(void)
{
    return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
}

uint16_t adcInternalReadTempsensor(void)
{
    return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
}
#endif

bool adcInit(void)
{
  bool ret = true;
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

    // RCC_ClockCmd(adc.rccADC, ENABLE);

    // ADC_CommonInitTypeDef ADC_CommonInitStructure;

    // ADC_CommonStructInit(&ADC_CommonInitStructure);
    // ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
    // ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div8;
    // ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
    // ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    // ADC_CommonInit(&ADC_CommonInitStructure);

// #ifdef USE_ADC_INTERNAL
//     // If device is not ADC1 or there's no active channel, then initialize ADC1 separately
//     if (device != ADCDEV_1 || !adcActive) {
//         RCC_ClockCmd(adcHardware[ADCDEV_1].rccADC, ENABLE);
//         adcInitDevice(ADC1, 2);
//         ADC_Cmd(ADC1, ENABLE);
//     }

//     // Initialize for injected conversion
//     adcInitInternalInjected(config);

//     if (!adcActive) {
//         return;
//     }
// #endif

    //adcInitDevice(adc.ADCx, configuredAdcChannels);

    // uint8_t rank = 1;
    // for (i = 0; i < ADC_CHANNEL_COUNT; i++) {
    //     if (!adcOperatingConfig[i].enabled) {
    //         continue;
    //     }
    //     ADC_RegularChannelConfig(adc.ADCx, adcOperatingConfig[i].adcChannel, rank++, adcOperatingConfig[i].sampleTime);
    // }
    // ADC_DMARequestAfterLastTransferCmd(adc.ADCx, ENABLE);

    // ADC_DMACmd(adc.ADCx, ENABLE);
    // ADC_Cmd(adc.ADCx, ENABLE);

// #ifdef USE_DMA_SPEC
//     const dmaChannelSpec_t *dmaSpec = dmaGetChannelSpecByPeripheral(DMA_PERIPH_ADC, device, config->dmaopt[device]);

//     if (!dmaSpec || !dmaAllocate(dmaGetIdentifier(dmaSpec->ref), OWNER_ADC, RESOURCE_INDEX(device))) {
//         return;
//     }

//     dmaEnable(dmaGetIdentifier(dmaSpec->ref));

//     xDMA_DeInit(dmaSpec->ref);
// #else
//     if (!dmaAllocate(dmaGetIdentifier(adc.dmaResource), OWNER_ADC, 0)) {
//         return;
//     }

//     dmaEnable(dmaGetIdentifier(adc.dmaResource));

//     xDMA_DeInit(adc.dmaResource);
// #endif

//     DMA_InitTypeDef DMA_InitStructure;

//     DMA_StructInit(&DMA_InitStructure);
//     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&adc.ADCx->DR;

// #ifdef USE_DMA_SPEC
//     DMA_InitStructure.DMA_Channel = dmaSpec->channel;
// #else
//     DMA_InitStructure.DMA_Channel = adc.channel;
// #endif

//     DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)adcValues;
//     DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//     DMA_InitStructure.DMA_BufferSize = configuredAdcChannels;
//     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//     DMA_InitStructure.DMA_MemoryInc = configuredAdcChannels > 1 ? DMA_MemoryInc_Enable : DMA_MemoryInc_Disable;
//     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//     DMA_InitStructure.DMA_Priority = DMA_Priority_High;

// #ifdef USE_DMA_SPEC
//     xDMA_Init(dmaSpec->ref, &DMA_InitStructure);
//     xDMA_Cmd(dmaSpec->ref, ENABLE);
// #else
//     xDMA_Init(adc.dmaResource, &DMA_InitStructure);
//     xDMA_Cmd(adc.dmaResource, ENABLE);
// #endif

//     ADC_SoftwareStartConv(adc.ADCx);
    return ret;
}

uint16_t adcGetChannel(uint8_t channel)
{
    adcGetChannelValues();

#ifdef DEBUG_ADC_CHANNELS
    if (adcOperatingConfig[0].enabled) {
        debug[0] = adcValues[adcOperatingConfig[0].dmaIndex];
    }
    if (adcOperatingConfig[1].enabled) {
        debug[1] = adcValues[adcOperatingConfig[1].dmaIndex];
    }
    if (adcOperatingConfig[2].enabled) {
        debug[2] = adcValues[adcOperatingConfig[2].dmaIndex];
    }
    if (adcOperatingConfig[3].enabled) {
        debug[3] = adcValues[adcOperatingConfig[3].dmaIndex];
    }
#endif
    return adcValues[adcOperatingConfig[channel].dmaIndex];
}

// Verify a pin designated by tag has connection to an ADC instance designated by device

// bool adcVerifyPin(ioTag_t tag, ADCDevice device)
// {
//     if (!tag) {
//         return false;
//     }

//     for (int map = 0 ; map < ADC_TAG_MAP_COUNT ; map++) {
// #if defined(STM32F1)
//         UNUSED(device);
//         if ((adcTagMap[map].tag == tag)) {
//             return true;
//         }
// #else
//         if ((adcTagMap[map].tag == tag) && (adcTagMap[map].devices & (1 << device))) {
//             return true;
//         }
// #endif
//     }

//     return false;
// }

#ifdef USE_ADC_INTERNAL

int32_t adcVREFINTCAL;      // ADC value (12-bit) of band gap with Vref = VREFINTCAL_VREF
int32_t adcTSCAL1;
int32_t adcTSCAL2;
int32_t adcTSSlopeK;

uint16_t adcInternalCompensateVref(uint16_t vrefAdcValue)
{
    // This is essentially a tuned version of
    // __HAL_ADC_CALC_VREFANALOG_VOLTAGE(vrefAdcValue, ADC_RESOLUTION_12B);
    return (uint16_t)((uint32_t)(adcVREFINTCAL * VREFINT_CAL_VREF) / vrefAdcValue);
}

int16_t adcInternalComputeTemperature(uint16_t tempAdcValue, uint16_t vrefValue)
{
    // This is essentially a tuned version of
    // __HAL_ADC_CALC_TEMPERATURE(vrefValue, tempAdcValue, ADC_RESOLUTION_12B);

    return ((((int32_t)((tempAdcValue * vrefValue) / TEMPSENSOR_CAL_VREFANALOG) - adcTSCAL1) * adcTSSlopeK) + 500) / 1000 + TEMPSENSOR_CAL1_TEMP;
}
#endif // USE_ADC_INTERNAL

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}