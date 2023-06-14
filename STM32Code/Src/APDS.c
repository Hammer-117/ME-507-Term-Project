/*!
 @file 		APDS.c
 @brief		This file provides the functionality to use an APDS9960 sensor for its color and proximity purposes.
 @author	Sean Wahl
 @date		June 14, 2023
*/

#include "APDS.h"
#include <stdio.h>

/*!
 @brief APDS9960 Object initialization function.
 This function configures an APDS struct to include all pertinent information for gathering data from the sensor
 such as the ADC integration time and the sensor output's gain value.
 @param 	hi2c: 		I2C handle pointer
 @param 	int_time: 	ADC integration time, see data sheet for specifics but 0xDB is around 100ms
 @param 	gain: 		Sensor output's gain (x1 is the only one calibrated to an 0xFF maximum)
 @retval 				Returns an APDS struct for future command use. 	
*/
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

    /* Power on and enable the sensor for proximity and color usage*/
    pkg[0] = 0x07;
    HAL_I2C_Mem_Write(myAPDS.hi2c, APDS9960_ADDR, APDS9960_ENABLE, 1, pkg, 1, HAL_MAX_DELAY);

    /* Spit out APDS object for use */
    return myAPDS;
}

/*!
 @brief APDS9960 Main color reading function
 This function accesses the APDS's memory registers to extract the red, green, and blue color values of what 
 is in front of it.
 @param 	myAPDS: 	APDS9960 struct type
 @retval 				Returns the APDS struct back, now with the RGB data to be accessed from its fields
*/
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

/*!
 @brief APDS9960 Data adjustment function
 This function takes the APDS's current color values and adjusts them to the standard scale (0xFF maximum).
 A byproduct of this normalization is that the highest color value present becomes 0xFF. This makes it impossible
 to read dark colors but works well for bright colors.
 @param 	myAPDS: 	APDS9960 struct type
 @retval 				Returns the APDS struct back, now with the adjusted RGB data to be accessed from its fields
*/
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

/*!
 @brief APDS9960 Color detection function
 This function utilizes the ReadColorData() and AdjustData() functions in order to determine what color it is looking
 at. These colors are limited to red, green, blue, and yellow. These colors have been assigned values 0-3, respectively.
 If it cannot determine if it's looking at a color it will return 4.
 @param 	myAPDS: 	APDS9960 struct type
 @retval 				Returns the number associated with the color sensed (RBGY - 0123)
*/
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

/*!
 @brief APDS9960 Proximity detection function
 This function accesses the APDS's proximity register and returns the value. Note: the proximity data ranges from 0x00 to 0xFF,
 with 0xFF being the shortest distance.
 @param 	myAPDS: 	APDS9960 struct type
 @retval 				Returns the proximity value from the APDS's register
*/
uint8_t APDS9960_Prox(APDS_t myAPDS) {
	uint8_t data;
	HAL_I2C_Mem_Read(myAPDS.hi2c, APDS9960_ADDR, APDS9960_PDATA, 1, &data, 1, HAL_MAX_DELAY);
	return data;
}
