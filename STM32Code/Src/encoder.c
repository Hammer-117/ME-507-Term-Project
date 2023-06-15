/*!
 @file 		encoder.c
 @brief		This file provides the ability to interface with an encoder. Note:
			timer channels 1 and 2 must be used to be able to utilize STM32 CubeIDE's
			Encoder Mode.
 @author	Sean Wahl
 @date		June 14, 2023
*/

#include "encoder.h"
#include "math.h"

/*!
 @brief Encoder initialization function.
 This function configures a struct to contain all necessary information for using an encoder,
 including the encoder's timer number, the auto reload value, and more. 
 @param 	htim: 		Timer handle pointer
 @param 	ARR: 		Auto reload value of the timer
 @param 	CPR: 		Counts per revolution of the encoder
 @param 	GR:			Gear ratio attached to the output of the encoder's motor
 @retval 				Returns an encoder struct for future command use. 	
*/
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

/*!
 @brief Encoder update and reading function.
 This function updates the encoder's count based on the change in the timer's value, accounting for over
 and under flow. This value is also available in radian format.
 @param 	myEnc:		Encoder struct type
 @retval 				Encoder struct for future use
*/
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

/*!
 @brief Encoder zeroing function
 This function resets the encoder's count to zero
 @param 	myEnc:		Encoder struct type
 @retval 				Encoder struct for future use
*/
encoder_t encoder_Zero(encoder_t myEnc) {
	myEnc.count = 0;
	myEnc.countR = 0;
	return myEnc;
}


