/*
 * APDS.c
 *
 *  Created on: May 27, 2023
 *      Author: seanw
 */

#include "APDS.h"
#include <stdio.h>

/* Initialize APDS9960 */
APDS_t APDS9960_Init(I2C_HandleTypeDef* hi2c, uint8_t int_time, uint8_t gain) {
	APDS_t myAPDS = {hi2c, 0, 0, 0};

    uint8_t pkg[1];

    /* Set integration time value, see data sheet for values */
    pkg[0] = int_time;
    HAL_I2C_Mem_Write(myAPDS.hi2c, APDS9960_ADDR, APDS9960_ATIME, 1, pkg, 1, HAL_MAX_DELAY);

    /* Set gain value, only accepts 0 to 3 */
    if (gain > x64) { gain = x64;}

    if (gain == x1) {myAPDS.GAIN = 1;}
    else if (gain == x4) {myAPDS.GAIN = 4;}
    else if (gain == x16) {myAPDS.GAIN = 16;}
    else if (gain == x64) {myAPDS.GAIN = 64;}
    pkg[0] = gain;
    HAL_I2C_Mem_Write(myAPDS.hi2c, APDS9960_ADDR, APDS9960_CONTROL, 1, pkg, 1, HAL_MAX_DELAY);

    /* Power on and enable the sensor */
    pkg[0] = 0x07;
    HAL_I2C_Mem_Write(myAPDS.hi2c, APDS9960_ADDR, APDS9960_ENABLE, 1, pkg, 1, HAL_MAX_DELAY);

    /* Spit out APDS object for use */
    return myAPDS;
}

/* Read color data from APDS9960 */
APDS_t APDS9960_ReadColorData(APDS_t myAPDS) {
    uint8_t data[6];

    /* Read color data registers */
    HAL_I2C_Mem_Read(myAPDS.hi2c, APDS9960_ADDR, APDS9960_RDATAL, 1, data, 6, HAL_MAX_DELAY);

    /* Extract color values */
    myAPDS.R = ((data[1] << 8) | data[0])/myAPDS.GAIN;
    myAPDS.G = ((data[3] << 8) | data[2])/myAPDS.GAIN;
    myAPDS.B = ((data[5] << 8) | data[4])/myAPDS.GAIN;

    return myAPDS;
}

APDS_t APDS9960_AdjustData(APDS_t myAPDS) {
	uint16_t max;
	if ((myAPDS.R > myAPDS.G) && (myAPDS.R > myAPDS.B)){
		max = myAPDS.R;
	}
	else if (myAPDS.G > myAPDS.B) {
		max = myAPDS.G;
	}
	else {
		max = myAPDS.B;
	}
	myAPDS.R = myAPDS.R * 0xFF / max;
	myAPDS.G = myAPDS.G * 0xFF / max;
	myAPDS.B = myAPDS.B * 0xFF / max;
	return myAPDS;
}

uint32_t APDS9960_Color(APDS_t myAPDS) {
	myAPDS = APDS9960_ReadColorData(myAPDS);
	if((myAPDS.R < 0x10) && (myAPDS.G < 0x10) && (myAPDS.B < 0x10)) {return 4;}
	myAPDS = APDS9960_AdjustData(myAPDS);

	if (myAPDS.B == 0xFF) {return 2;}
	else if ((myAPDS.R > 0xD0) && (myAPDS.G > 0xD0)) {return 3;}
	else if (myAPDS.R == 0xFF) {return 0;}
	else if (myAPDS.G == 0xFF) {return 1;}
	else {return 4;}
}

uint8_t APDS9960_Prox(APDS_t myAPDS) {
	uint8_t data;
	HAL_I2C_Mem_Read(myAPDS.hi2c, APDS9960_ADDR, APDS9960_PDATA, 1, &data, 1, HAL_MAX_DELAY);
	return data;
}
