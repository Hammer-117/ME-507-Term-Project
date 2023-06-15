/*!
 @file 		TCRT.h
 @brief		This is a header file for TCRT.c, the line follower driver.
 @author	Sean Wahl
 @date		June 14, 2023
*/

#ifndef INC_TCRT_H_
#define INC_TCRT_H_

#include "stm32f4xx_hal.h"

/*! @struct TCRTType
 	@brief 	This structure contains the pertinent information for using and reading data
			from a TCRT sensor and should be used like an object.
 */
typedef struct TCRTType {
		ADC_HandleTypeDef*  hadc;		/*!< ADC handle pointer */
		uint32_t            channel;	/*!< ADC channel number attached to analog signal */
		GPIO_TypeDef*		Dig_Port;	/*!< GPIO Port attached to digital pin */
		uint16_t			Dig_Pin;	/*!< GPIO Pin attached to digital pin */
		uint16_t			Analog;		/*!< Analog reading from sensor */
		uint16_t			Digital;	/*!< Digital reading from sensor */
	} TCRT_t;



void TCRT_Error(void);

TCRT_t TCRT_Init(ADC_HandleTypeDef* hadc, uint32_t channel, GPIO_TypeDef* Dig_Port, uint16_t Dig_Pin);
TCRT_t TCRT_ReadAll(TCRT_t myTCRT);
TCRT_t TCRT_ReadAnalog(TCRT_t myTCRT);
TCRT_t TCRT_ReadDigital(TCRT_t myTCRT);


#endif /* INC_TCRT_H_ */
