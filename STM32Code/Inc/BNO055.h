/*
 * BNO055.h
 *
 *  Created on: May 28, 2023
 *      Author: seanw
 */

#ifndef INC_BNO055_H_
#define INC_BNO055_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>

typedef struct BNO055Type {
		I2C_HandleTypeDef*  hi2c;
		uint8_t				opr_mode;
		uint8_t				mag_stat;
		uint8_t				acc_stat;
		uint8_t				gyr_stat;
		uint8_t				sys_stat;
		int16_t				eul_heading;
		int16_t				eul_roll;
		int16_t				eul_pitch;
	} BNO055_t;

// Device address
#define BNO055_ADDR 0x50

// Important registers
#define BNO055_OPR_MODE          0x3D
#define BNO055_CALIB_STAT        0x35
#define BNO055_ACC_OFFSET_X_LSB  0x55
#define BNO055_EUL_Heading_LSB   0x1A
#define BNO055_GYR_DATA_X_LSB    0x14

// Operating modes
#define CONFIGMODE    0x00
#define ACC_ONLY      0x01
#define MAG_ONLY      0x02
#define GYRO_ONLY     0x03
#define ACC_MAG       0x04
#define ACC_GYRO      0x05
#define MAG_GYRO      0x06
#define AMG           0x07
#define IMU           0x08
#define COMPASS       0x09
#define M4G           0x0A
#define NDOF_FMC_OFF  0x0B
#define NDOF          0x0C

// Function Prototypes
BNO055_t BNO_Init(I2C_HandleTypeDef* hi2c);
BNO055_t BNO_OprMode(BNO055_t myBNO, uint8_t mode);
BNO055_t BNO_CalStat(BNO055_t myBNO);
BNO055_t BNO_GetEul(BNO055_t myBNO);
//BNO055_t BNO_GetAcc(BNO055_t myBNO);
//BNO055_t BNO_GetGyr(BNO055_t myBNO);

#endif /* INC_BNO055_H_ */
