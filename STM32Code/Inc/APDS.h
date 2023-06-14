/*
 * APDS.h
 *
 *  Created on: May 27, 2023
 *      Author: seanw
 */

#ifndef INC_APDS_H_
#define INC_APDS_H_

#include "stm32f4xx_hal.h"

typedef struct APDSType {
		I2C_HandleTypeDef*  hi2c;
		uint16_t			R;
		uint16_t			G;
		uint16_t 			B;
		uint32_t			GAIN;
	} APDS_t;

/* APDS9960 Registers */
#define APDS9960_ADDR               0x72

#define APDS9960_ENABLE             0x80
#define APDS9960_ATIME              0x81
#define APDS9960_CONTROL            0x8F
#define APDS9960_STATUS             0x93
#define APDS9960_RDATAL             0x96
#define APDS9960_RDATAH             0x97
#define APDS9960_GDATAL             0x98
#define APDS9960_GDATAH             0x99
#define APDS9960_BDATAL             0x9A
#define APDS9960_BDATAH             0x9B
#define APDS9960_PDATA				0x9C

/* Hungry Hippo Ball Colors */
#define RED			0
#define GREEN		1
#define BLUE		2
#define YELLOW		3

/* ALS Control Gains */
#define x1		0x00
#define x4 		0x01
#define x16		0x02
#define x64		0x03

/* Function Prototypes */
APDS_t APDS9960_Init(I2C_HandleTypeDef* hi2c, uint8_t int_time, uint8_t gain);
APDS_t APDS9960_ReadColorData(APDS_t my_APDS);
APDS_t APDS9960_AdjustData(APDS_t myAPDS);
uint32_t APDS9960_Color(APDS_t myAPDS);
uint8_t APDS9960_Prox(APDS_t myAPDS);


#endif /* SRC_APDS_H_ */
