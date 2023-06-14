/*
 * TCRT.h
 *
 *  Created on: May 28, 2023
 *      Author: seanw
 */

#ifndef INC_TCRT_H_
#define INC_TCRT_H_

#include "stm32f4xx_hal.h"

typedef struct TCRTType {
		ADC_HandleTypeDef*  hadc;
		uint32_t            channel;
		GPIO_TypeDef*		Dig_Port;
		uint16_t			Dig_Pin;
		uint16_t			Analog;
		uint16_t			Digital;
	} TCRT_t;



void TCRT_Error(void);

TCRT_t TCRT_Init(ADC_HandleTypeDef* hadc, uint32_t channel, GPIO_TypeDef* Dig_Port, uint16_t Dig_Pin);
TCRT_t TCRT_ReadAll(TCRT_t myTCRT);
TCRT_t TCRT_ReadAnalog(TCRT_t myTCRT);
TCRT_t TCRT_ReadDigital(TCRT_t myTCRT);


#endif /* INC_TCRT_H_ */
