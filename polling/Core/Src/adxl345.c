/*
 * adxl345.c
 *
 *  Created on: Jan 10, 2026
 *      Author: ksiaz
 */


#include "adxl345.h"
#include <string.h>


extern SPI_HandleTypeDef hspi1;//to odniesienie jakby do tego w main co idle generuje


//makra dla porzadku przy wywolywaniu stanow CS podczas odczytów
#define ADXL_CS_LOW() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
#define ADXL_CS_HIGH() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);


void ADXL345_WriteRegister(uint8_t reg, uint8_t value)//wpisujemy wartosci do rejestrow potrzebne do ustalenia trubow pracy itd
{
    uint8_t data[2];
    data[0] = reg & 0x3F;
    data[1] = value;

    ADXL_CS_LOW();//poczatek transmisji
    HAL_SPI_Transmit(&hspi1, data, 2, 100);//wlasciwa transmisja
    ADXL_CS_HIGH();//koniec stany z datasheeta
}

uint8_t ADXL345_ReadRegister(uint8_t reg)//odczytujemy rejestry po to zeby wiedziec co sie dzieje
{
    uint8_t txData[2], rxData[2];//def wysylanych i odbieranych danych
    txData[0] = (reg & 0x3F) | 0x80;//ustawiamy
    txData[1] = 0x00;//pusty wartosc zeby odebrac trzeba wyslac

    ADXL_CS_LOW();
    HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, 100);
    ADXL_CS_HIGH();

    return rxData[1];
}

void ADXL345_ReadAccel(int16_t *x, int16_t *y, int16_t *z)//odczyt danych na wsakznikach bo potem uzywamy
{
    uint8_t txData[7] = {0};
    uint8_t rxData[7] = {0};

    txData[0] = (ADXL345_REG_DATAX0 & 0x3F) | 0xC0;//wartosci wysylane zeby odczytac po SPI

    ADXL_CS_LOW();
    HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 7, 100);
    ADXL_CS_HIGH();

    *x = (int16_t)((rxData[2] << 8) | rxData[1]);//przesuneicie bitowe rzeby miec 16 bitowe slowa bo adxl wysyla w bajtach
    *y = (int16_t)((rxData[4] << 8) | rxData[3]);
    *z = (int16_t)((rxData[6] << 8) | rxData[5]);
}

void ADXL345_Init(SPI_HandleTypeDef *hspi)//inicjalizacja aby poprawnie dziala tzn tak jak oczekujemy
{

	    ADXL345_WriteRegister(ADXL345_REG_POWER_CTL, 0x00);//właczenie czujnika
	    HAL_Delay(10);
	    ADXL345_WriteRegister(ADXL345_REG_DATA_FORMAT, 0x08);//
	    ADXL345_WriteRegister(ADXL345_REG_BW_RATE, 0x0A);//ustawienie czestotliwosci
	    ADXL345_WriteRegister(ADXL345_REG_INT_ENABLE, 0x00);//wyłącznei przerwan
	    ADXL345_WriteRegister(ADXL345_REG_POWER_CTL, ADXL345_POWER_CTL_MEASURE);//
	    HAL_Delay(10);
}
