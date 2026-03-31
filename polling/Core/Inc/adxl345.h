/*
 * adxl345.h
 *
 *  Created on: Jan 10, 2026
 *      Author: ksiaz
 */

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

#include "main.h"


#define ADXL345_REG_DEVID 0x00
#define ADXL345_REG_BW_RATE 0x2C
#define ADXL345_REG_POWER_CTL 0x2D
#define ADXL345_REG_INT_ENABLE 0x2E
#define ADXL345_REG_DATA_FORMAT 0x31
#define ADXL345_REG_DATAX0 0x32

#define ADXL345_POWER_CTL_MEASURE 0x08

#define ADXL345_REG_INT_MAP 0x2F
#define ADXL345_INT_DATA_READY 0x80

void ADXL345_Init(SPI_HandleTypeDef *hspi);
uint8_t ADXL345_ReadRegister(uint8_t reg);
void ADXL345_WriteRegister(uint8_t reg, uint8_t value);
void ADXL345_ReadAccel(int16_t *x, int16_t *y, int16_t *z);

#endif /* INC_ADXL345_H_ */
