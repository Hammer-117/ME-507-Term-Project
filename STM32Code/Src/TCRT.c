/*!
 @file 		TCRT.c
 @brief		This file provides the functionality to use a TCRT line follower.
 @author	Sean Wahl
 @date		June 14, 2023
*/

#include "TCRT.h"


void TCRT_Error(void) {
  __disable_irq();
  while (1)
  { }
}

/*!
 @brief TCRT Object initialization function.
 This function configures an TCRT struct to include all pertinent information for gathering data from the sensor.
 @param 	hadc: 		ADC handle pointer
 @param 	channel: 	ADC channel attached to the connected analog pin
 @param 	Dig_Port: 	GPIO Port connected to the digital pin
 @param		Dig_Pin:	GPIO Pin number of the connected digital pin
 @retval 				Returns a TCRT struct for future command use. 	
*/
TCRT_t TCRT_Init(ADC_HandleTypeDef* hadc, uint32_t channel, GPIO_TypeDef* Dig_Port, uint16_t Dig_Pin) {
	TCRT_t myTCRT;
	myTCRT.hadc = hadc;
	myTCRT.channel = channel;
	myTCRT.Dig_Port = Dig_Port;
	myTCRT.Dig_Pin = Dig_Pin;

	return myTCRT;
}

/*!
 @brief TCRT Full read function.
 This function reads both the digital and analog reading from the TCRT and puts them in the TCRT struct
 for accessibility.
 @param 	myTCRT:		TCRT type struct
 @retval 				Returns a TCRT struct for accessing values
*/
TCRT_t TCRT_ReadAll(TCRT_t myTCRT) {
	myTCRT = TCRT_ReadAnalog(myTCRT);
	myTCRT = TCRT_ReadDigital(myTCRT);
	return myTCRT;
}

/*!
 @brief TCRT Analog read function.
 This function reads only the analog reading from the TCRT and puts it in the TCRT struct
 for accessibility.
 @param 	myTCRT:		TCRT type struct
 @retval 				Returns a TCRT struct for accessing values
*/
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

/*!
 @brief TCRT Digital read function
 This function reads only the digital reading from the TCRT and puts it in the TCRT struct
 for accessibility. Note: This value's threshold is controlled through a potentiometer on 
 the TCRT.
 @param 	myTCRT:		TCRT type struct
 @retval 				Returns a TCRT struct for accessing values
*/
TCRT_t TCRT_ReadDigital(TCRT_t myTCRT) {
	myTCRT.Digital = HAL_GPIO_ReadPin(myTCRT.Dig_Port, myTCRT.Dig_Pin);
	return myTCRT;
}



