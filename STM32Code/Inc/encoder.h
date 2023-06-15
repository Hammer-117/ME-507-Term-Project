/*!
 @file 		encoder.h
 @brief		This is a header file for encoder.c, the encoder driver.
 @author	Sean Wahl
 @date		June 14, 2023
*/

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>

/*! @struct EncoderType
 	@brief 	This structure contains the pertinent information for using and reading data
			from an encoder.
 */
typedef struct EncoderType {
		TIM_HandleTypeDef*  htim; 		/*!< The timer handle pointer */
		int64_t		    	ARR; 		/*!< The timer's auto reload value */
		int32_t				c2r;		/*!< The encoder's count to revolution conversion */
		int32_t				prev;		/*!< The timer's previous reading */
		int32_t 			count;		/*!< The encoder's count in ticks */
		float				countR;		/*!< The encoder's count in radians */
	} encoder_t;

encoder_t encoder_Init(TIM_HandleTypeDef* htim, uint32_t ARR, uint32_t CPR, uint32_t GR);
encoder_t encoder_Update(encoder_t myEnc);
encoder_t encoder_Zero(encoder_t myEnc);



#endif /* INC_ENCODER_H_ */
