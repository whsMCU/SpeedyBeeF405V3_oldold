/*
 * hw.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "hw.h"

static void MX_DMA_Init(void);

void hwInit(void)
{
  bspInit();
  #ifdef _USE_HW_RTC
    rtcInit();
  #endif
  gpioInit();
  flashInit();
  ledInit();
  MX_DMA_Init();
  usbInit();
  uartInit();
  cliInit();
  i2cInit();
  spiInit();
  adcInit();
  
  if (sdInit() == true)
  {
    fatfsInit();
  }


	// ledOn(ST1);
  // ledOff(ST2);
  // for (int i = 0; i < 10; i++){
  //   ledToggle(ST1);
  //   ledToggle(ST2);
  //   HAL_Delay(25);
  //   //BEEP_ON;
  //   HAL_Delay(25);
  //   //BEEP_OFF;
  // }
  // ledOff(ST1);
  // ledOff(ST2);
}


/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

	  /* DMA controller clock enable */
	  __HAL_RCC_DMA1_CLK_ENABLE();
	  __HAL_RCC_DMA2_CLK_ENABLE();

	  /* DMA interrupt init */
	  /* DMA1_Stream0_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	  /* DMA1_Stream1_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	  /* DMA1_Stream2_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
	  /* DMA1_Stream3_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	  /* DMA1_Stream4_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	  /* DMA1_Stream5_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	  /* DMA2_Stream0_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	  /* DMA2_Stream1_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	  /* DMA2_Stream2_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	  /* DMA2_Stream3_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
	  /* DMA2_Stream5_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

}
