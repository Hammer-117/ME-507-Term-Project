/*
 * motorDriver.h
 *
 *  Created on: Apr 20, 2023
 *      Author: seanw
 */

#ifndef MOTORDRIVER_H_
#define MOTORDRIVER_H_

#include "stm32f4xx_hal.h"

typedef struct motorType {
		TIM_HandleTypeDef*  htim;
		uint32_t			ch1;
		uint32_t			ch2;
		uint32_t 			ARp1;
	} motorType_t;

void setDuty(motorType_t, int32_t DTY);
void enableMotor(motorType_t motor);
void disableMotor(motorType_t motor);
void brakeMotor(motorType_t motor);

#endif /* MOTORDRIVER_H_ */
