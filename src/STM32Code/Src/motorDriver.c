/*!
 @file 		motorDriver.c
 @brief		This file provides the functionality to control and motor with PWM on an H-bridge.
 @author	Sean Wahl
 @date		June 14, 2023
*/

#include "motorDriver.h"

/*!
 @brief Sets the duty cycle of a motor.
 This function takes a value from 0 to 255 (in int8) to apply PWM to drive a motor.
 @param 	motor:	Motor struct type
 @param		DTY:	The desired duty cycle of the motor
*/
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

/*!
 @brief Enables the motor.
 This function activates the PWM channels for the motor to run.
 @param 	motor:	Motor struct type
*/
void enableMotor(motorType_t motor) {
	HAL_TIM_PWM_Start(motor.htim, motor.ch1);
	HAL_TIM_PWM_Start(motor.htim, motor.ch2);
}

/*!
 @brief Disables the motor.
 This function deactivates the PWM channels for the motor to cease to function.
 @param 	motor:	Motor struct type
*/
void disableMotor(motorType_t motor) {
	HAL_TIM_PWM_Stop(motor.htim, motor.ch1);
	HAL_TIM_PWM_Stop(motor.htim, motor.ch2);
}

/*!
 @brief Brakes the motor.
 This function sets the duty cycles to their maximums in order to slow down the
 motor quickly.
 @param 	motor:	Motor struct type
*/
void brakeMotor(motorType_t motor) {
	__HAL_TIM_SET_COMPARE(motor.htim, motor.ch1, motor.ARp1);
	__HAL_TIM_SET_COMPARE(motor.htim, motor.ch2, motor.ARp1);
}
