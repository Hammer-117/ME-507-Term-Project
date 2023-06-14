/*
 * encoder.c
 *
 *  Created on: May 29, 2023
 *      Author: seanw
 */

#include "encoder.h"
#include "math.h"

encoder_t encoder_Init(TIM_HandleTypeDef* htim, uint32_t ARR, uint32_t CPR, uint32_t GR) {
	encoder_t myEnc;
	myEnc.htim = htim;
	HAL_TIM_Encoder_Start(myEnc.htim, TIM_CHANNEL_ALL);
	myEnc.prev = __HAL_TIM_GET_COUNTER(myEnc.htim);
	myEnc.count = 0;
	myEnc.countR = 0;

	myEnc.ARR = ARR;
	myEnc.c2r = 4 * CPR * GR;

	return myEnc;
}


encoder_t encoder_Update(encoder_t myEnc) {
	int32_t timVal = __HAL_TIM_GET_COUNTER(myEnc.htim);
	int32_t diff = timVal - myEnc.prev;
	myEnc.prev = timVal;

	if (diff > (myEnc.ARR/2)) {
		myEnc.count += diff - myEnc.ARR;
	}
	else if ((-diff) > (myEnc.ARR/2)) {
		myEnc.count += diff + myEnc.ARR;
	}
	else {
		myEnc.count += diff;
	}
	myEnc.countR = myEnc.count * 2 * M_PI / myEnc.c2r;
	return myEnc;
}

encoder_t encoder_Zero(encoder_t myEnc) {
	myEnc.count = 0;
	myEnc.countR = 0;
	return myEnc;
}


