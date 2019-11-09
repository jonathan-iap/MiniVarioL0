/*
 * MS56xx.H
 *
 *  Created on: 27 janv. 2019
 *      Author: jo
 */

#ifndef PERIPHERALS_MS56xx_H_
#define PERIPHERALS_MS56xx_H_

#include "main.h"

typedef enum
{
	OSR_ULTRA_LOW_POWER  = 0x00,
	OSR_LOW_POWER        = 0x02,
	OSR_STANDARD         = 0x04,
	OSR_HIGH_RES         = 0x06,
	OSR_ULTRA_HIGH_RES   = 0x08
} ms5611_osr_t;

enum
{
	CMD_RESET = 0x1E,

	CMD_CONV_D1  = 0x40,
	CMD_CONV_D2  = 0x50,

	CMD_READ_ADC  = 0x00,
	CMD_READ_PROM = 0xA0,
	CMD_READ_CRC  = 0xAE
};

// USER
void MS56xx_Init();
double MS56xx_getPressure(ms5611_osr_t _osrPr, ms5611_osr_t _osrTemp);
double MS56xx_getTemperature(void);
double ms56xx_getAltitude(double pressure);

// PRIVATE
void MS56xx_Reset(void);
void MS56xx_ReadPROM(void);
uint8_t MS56xx_crc4(uint16_t n_prom[]);
uint32_t MS56xx_Read_RAW_ADC(char cmd_conv, ms5611_osr_t osr);

#endif /* PERIPHERALS_MS56xx_H_ */
