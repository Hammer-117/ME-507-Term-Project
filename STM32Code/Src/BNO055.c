/*
 * IMU.c
 *
 *  Created on: May 29, 2023
 *      Author: seanw
 */

#include "BNO055.h"

BNO055_t BNO_Init(I2C_HandleTypeDef* hi2c) {
	BNO055_t myBNO;
	myBNO.hi2c = hi2c;

	myBNO = BNO_OprMode(myBNO, CONFIGMODE);

	return myBNO;
}


BNO055_t BNO_OprMode(BNO055_t myBNO, uint8_t mode) {
	uint8_t data[1];
	data[0] = CONFIGMODE;
	HAL_I2C_Mem_Write(myBNO.hi2c, BNO055_ADDR, BNO055_OPR_MODE, 1, data, 1, HAL_MAX_DELAY);
	HAL_Delay(20);
	if (mode != CONFIGMODE) {
		data[0] = mode;
		HAL_I2C_Mem_Write(myBNO.hi2c, BNO055_ADDR, BNO055_OPR_MODE, 1, data, 1, HAL_MAX_DELAY);
		HAL_Delay(8);
	}

	myBNO.opr_mode = mode;
	return myBNO;
}

BNO055_t BNO_CalStat(BNO055_t myBNO) {
	uint8_t cal_byte;

	HAL_I2C_Mem_Read(myBNO.hi2c, BNO055_ADDR, BNO055_CALIB_STAT, 1, &cal_byte, 1, HAL_MAX_DELAY);

	myBNO.mag_stat = (cal_byte & 0x03);
	myBNO.acc_stat = (cal_byte & 0x0C)>>2;
	myBNO.gyr_stat = (cal_byte & 0x30)>>4;
	myBNO.sys_stat = (cal_byte & 0xC0)>>6;

	return myBNO;
}

BNO055_t BNO_GetEul(BNO055_t myBNO) {
	uint8_t mag_bytes[6];

	HAL_I2C_Mem_Read(myBNO.hi2c, BNO055_ADDR, BNO055_EUL_Heading_LSB, 1, mag_bytes, 6, HAL_MAX_DELAY);

	myBNO.eul_heading = ((mag_bytes[1]<<8)|mag_bytes[0]);
	myBNO.eul_heading = myBNO.eul_heading/16 - 180;
	myBNO.eul_roll = ((mag_bytes[3]<<8)|mag_bytes[2]);
	myBNO.eul_roll = myBNO.eul_roll/16;
	myBNO.eul_pitch = ((mag_bytes[5]<<8)|mag_bytes[4]);
	myBNO.eul_pitch = myBNO.eul_pitch/16;

	return myBNO;
}


//BNO055_t BNO_GetAcc(BNO055_t myBNO);


//BNO055_t BNO_GetGyr(BNO055_t myBNO);



