/*
 * motorDriver.c
 *
 *  Created on: Apr 20, 2023
 *      Author: seanw
 */

#include "motorDriver.h"

void setDuty(motorType_t motor, int32_t DTY) {
	if (DTY >= 0) {
		if (DTY > 127) {
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch1, motor.ARp1);
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch2, 0);
		}
		else{
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch1, DTY*motor.ARp1/128);
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch2, 0);
		}
	}

	else
		if (DTY < -128) {
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch1, 0);
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch2, motor.ARp1);
				}
		else {
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch1, 0);
			__HAL_TIM_SET_COMPARE(motor.htim, motor.ch2, -DTY*motor.ARp1/128);
	}
}

void enableMotor(motorType_t motor) {
	HAL_TIM_PWM_Start(motor.htim, motor.ch1);
	HAL_TIM_PWM_Start(motor.htim, motor.ch2);
}

void disableMotor(motorType_t motor) {
	HAL_TIM_PWM_Stop(motor.htim, motor.ch1);
	HAL_TIM_PWM_Stop(motor.htim, motor.ch2);
}

void brakeMotor(motorType_t motor) {
	__HAL_TIM_SET_COMPARE(motor.htim, motor.ch1, motor.ARp1);
	__HAL_TIM_SET_COMPARE(motor.htim, motor.ch2, motor.ARp1);
}
