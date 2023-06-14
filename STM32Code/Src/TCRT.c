/*
 * TCRT.c
 *
 *  Created on: May 28, 2023
 *      Author: seanw
 */

#include "TCRT.h"

void TCRT_Error(void) {
  __disable_irq();
  while (1)
  { }
}

TCRT_t TCRT_Init(ADC_HandleTypeDef* hadc, uint32_t channel, GPIO_TypeDef* Dig_Port, uint16_t Dig_Pin) {
	TCRT_t myTCRT;
	myTCRT.hadc = hadc;
	myTCRT.channel = channel;
	myTCRT.Dig_Port = Dig_Port;
	myTCRT.Dig_Pin = Dig_Pin;

	return myTCRT;
}

TCRT_t TCRT_ReadAll(TCRT_t myTCRT) {
	myTCRT = TCRT_ReadAnalog(myTCRT);
	myTCRT = TCRT_ReadDigital(myTCRT);
	return myTCRT;
}

TCRT_t TCRT_ReadAnalog(TCRT_t myTCRT) {
	ADC_ChannelConfTypeDef sConfig = {0};

	// Configure ADC channel
	sConfig.Channel = myTCRT.channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;

	// Apply ADC configuration
	if (HAL_ADC_ConfigChannel(myTCRT.hadc, &sConfig) != HAL_OK)
	{
	TCRT_Error();
	}

	// Start ADC conversion
	if (HAL_ADC_Start(myTCRT.hadc) != HAL_OK)
	{
	TCRT_Error();
	}

	// Wait for conversion to complete
	if (HAL_ADC_PollForConversion(myTCRT.hadc, 100) != HAL_OK)
	{
	TCRT_Error();
	}

	// Read ADC value
	myTCRT.Analog = HAL_ADC_GetValue(myTCRT.hadc);
	return myTCRT;
}

TCRT_t TCRT_ReadDigital(TCRT_t myTCRT) {
	myTCRT.Digital = HAL_GPIO_ReadPin(myTCRT.Dig_Port, myTCRT.Dig_Pin);
	return myTCRT;
}



