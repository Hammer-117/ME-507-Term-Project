/*!
 @file 		motorDriver.h
 @brief		This is a header file for motorDriver.c, the motor driver.
 @author	Sean Wahl
 @date		June 14, 2023
*/

#ifndef MOTORDRIVER_H_
#define MOTORDRIVER_H_

#include "stm32f4xx_hal.h"

/*! @struct motorType
 	@brief 	This structure contains the pertinent information for using a motor.
 */
typedef struct motorType {
		TIM_HandleTypeDef*  htim;	/*!< The motor's timer handle */
		uint32_t			ch1;	/*!< The timer channel of the motor's positive terminal */
		uint32_t			ch2; 	/*!< The timer channel of the motor's negative terminal */
		uint32_t 			ARp1;	/*!< The timer's auto reload value plus one */
	} motorType_t;

void setDuty(motorType_t, int32_t DTY);
void enableMotor(motorType_t motor);
void disableMotor(motorType_t motor);
void brakeMotor(motorType_t motor);

#endif /* MOTORDRIVER_H_ */
