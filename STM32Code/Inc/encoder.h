/*
 * encoder.h
 *
 *  Created on: May 29, 2023
 *      Author: seanw
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>

typedef struct EncoderType {
		TIM_HandleTypeDef*  htim;
		int64_t		    	ARR;
		int32_t				c2r;
		int32_t				prev;
		int32_t 			count;
		float				countR;
	} encoder_t;

encoder_t encoder_Init(TIM_HandleTypeDef* htim, uint32_t ARR, uint32_t CPR, uint32_t GR);
encoder_t encoder_Update(encoder_t myEnc);
encoder_t encoder_Zero(encoder_t myEnc);



#endif /* INC_ENCODER_H_ */
