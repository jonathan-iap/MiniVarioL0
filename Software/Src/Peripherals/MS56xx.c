/*
 * MS56xx.c
 *
 *  Created on: 27 janv. 2019
 *      Author: jo
 */
#include "Peripherals/MS56xx.h"
#include "spi.h"
#include "usart.h"
#include "stdio.h"
#include "math.h"

#define SEA_PRESS   1013.25 //default sea level pressure level in mbar
#define FTMETERS    0.3048  //convert feet to meters

#define SIZE_CMD   1 //8bits
#define SIZE_PROM  2 //16bits
#define SIZE_ADC   3 //24bits

#define RELEAOD 3 // wait for new command after reset command (2,8ms in datasheet)

#define NB_OF_CF_FACTORY 8

/**
 * Calibration data :
 * [0] Reserved for manufacturer
 * [1] C1 : Pressure sensitivity  SENS
 * [2] C2 : Pressure offset  OFF
 * [3] C3 : Temperature coefficient of pressure sensitivity  TCS
 * [4] C4 : Temperature coefficient of pressure offset  TCO
 * [5] C5 : Reference temperature
 * [6] C6 : Temperature coefficient of the temperature
 * [7] CRC
 */
uint16_t _coefficientFactory[NB_OF_CF_FACTORY] = {0};

typedef struct
{
	uint32_t D1;  // Digital pressure value
	uint32_t D2;  // Digital temperature value

	double dT;   // Difference between actual and reference temperature
	double TEMP; // Actual temperature

	double OFF;  // Offset at actual temperature
	double SENS; // Sensitivity at actual temperature
	double P;    // Temperature compensated pressure (10...1200mbar with0.01mbar resolution)

}MS56xx_t;

MS56xx_t MS56xx;

/**
 * @brief  Initialization sequence
 * @param  None
 * @retval None
 */
void MS56xx_Init()
{
	double pressure = 0.00;
	double temperature = 0.00;

	printf("\nSensor check : \n");

	MS56xx_Reset();

	HAL_Delay(RELEAOD);

	MS56xx_ReadPROM();

	printf("CRC : 0x%x\n", MS56xx_crc4(_coefficientFactory));

	pressure = MS56xx_getPressure(OSR_HIGH_RES, OSR_HIGH_RES);
	temperature = MS56xx_getTemperature();

	printf("Pressure    : %.2f mBar\n",pressure);
	printf("Temperature : %.2f C\n"    ,temperature);

	if(pressure < 10.00 || pressure > 1200.00 || temperature < -40.00 || temperature > 85.00)
	{
		printf("** ERROR ! **\n");
		printf("Sensor FAIL : Caution sensor can be damage !\n");

		if(pressure<10.00) printf("Pressure value is under 10.00 mBar\n\n");
		else if (pressure > 1200.00) printf("Pressure value is over 1200.00 mBar\n\n");
		else if (temperature < -40.00) printf("Temperature value is under -40.00 C\n\n");
		else printf("Temperature value is over 85.00 C\n\n");
	}
	else printf("Sensor OK !\n\n");
}


/**
 * @brief  The Reset sequence shall be sent once after power-on to make sure that the calibration PROM
 * gets loaded into the internal register. It can be also used to reset the device ROM from an unknown condition
 * @param  None
 * @retval None
 */
void MS56xx_Reset(void)
{
	uint8_t command = CMD_RESET;

	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);

	HAL_SPI_Transmit(&hspi1, &command, SIZE_CMD, 500);

	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
}


/**
 * @brief The read command for PROM shall be executed once after reset by the user
 * to read the content of the calibration PROM and to calculate the calibration coefficients.
 * There are in total 8 addresses resulting in a total memory of 128 bit.
 * Address 0 contains factory data and the setup, addresses 1-6 calibration coefficients and
 * address 7 contains the serial code and CRC.
 * The command sequence is 8 bits long with a 16 bit result which is clocked with the MSB first.
 * @param  None
 * @retval None
 */
void MS56xx_ReadPROM(void)
{
	uint8_t command = 0;
	uint8_t data[3] = {0};
	/* data[3] is in link with the function HAL_SPI_TransmitReceive().
	 * The size of data receive is the same that is transmit.
	 * data[0] is the command
	 * data[1] and data[2] are the MSB and LSB data byte.
	 */

	printf("Reading PROM : \n");

	for(uint8_t addr = 0; addr < NB_OF_CF_FACTORY ; addr++)
	{
		command = (CMD_READ_PROM + (addr * 2));

		// Read calibration registerX
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);

		HAL_SPI_TransmitReceive(&hspi1, &command, data, (SIZE_CMD+SIZE_PROM), 500); // Instruction faster than transmit + receive

		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);

		// Convert and store into 16 bits value
		_coefficientFactory[addr] = (data[1] << 8) | data[2];

		printf("Addr[%d] : %d \n", addr, _coefficientFactory[addr]);
	}
}


/*
 * @brief calculate the CRC code for check the data PROM validity
 * @param  None
 * @retval crc code
 */
uint8_t MS56xx_crc4(uint16_t n_prom[])
{
	uint8_t cnt;       // simple counter
	uint16_t n_rem;    // crc reminder
	uint16_t crc_read; // original value of the crc
	uint8_t  n_bit;

	n_rem = 0x00;
	crc_read = n_prom[7];               //save read CRC
	n_prom[7] = (0xFF00 & (n_prom[7])); //CRC byte is replaced by 0

	for (cnt = 0; cnt < 16; cnt++)   // operation is performed on bytes
	{
		// choose LSB or MSB
		if (cnt%2==1) n_rem ^= (uint8_t) ((n_prom[cnt>>1]) & 0x00FF);
		else n_rem ^= (uint8_t) (n_prom[cnt>>1]>>8);

		for (n_bit = 8; n_bit > 0; n_bit--)
		{
			if (n_rem & (0x8000))
			{
				n_rem = (n_rem << 1) ^ 0x3000;
			}
			else
			{
				n_rem = (n_rem << 1);
			}
		}
	}

	n_rem = (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code

	n_prom[7] = crc_read; // restore the crc_read to its original place

	return (n_rem ^ 0x00);
}

/**
 * @brief  read uncompensated data and convert into 32bits value
 * @param  reg : register address
 * @param  osr : oversampling value
 * @retval value : uncompensated value of the register in form of 32bits
 */
uint32_t MS56xx_Read_RAW_ADC(char cmd_conv, ms5611_osr_t osr)
{
	uint8_t convTime = 0;  // Wait time for conversion depending of oversampling in ms

	uint8_t cmd_convert  = cmd_conv + osr; // pressure/temperature addr + OSR
	uint8_t cmd_read_adc = CMD_READ_ADC;

	uint32_t value;
	uint8_t data[4] = {0};
	/* data[4] is in link with the function HAL_SPI_TransmitReceive().
	 * The size of data receive is the same that is transmit.
	 * data[0] is the command
	 * data[1], data[2] and data[3] are the MSB and LSB data byte.
	 */

	switch (osr)
	{
	case OSR_ULTRA_LOW_POWER:
		convTime = 1;
		break;
	case OSR_LOW_POWER:
		convTime = 3;
		break;
	case OSR_STANDARD:
		convTime = 4;
		break;
	case OSR_HIGH_RES:
		convTime = 6;
		break;
	case OSR_ULTRA_HIGH_RES:
		convTime = 10;
		break;
	}

	// Convertion request
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);

	HAL_SPI_Transmit(&hspi1, &cmd_convert, SIZE_CMD, 500);

	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);

	// wait for convertion
	HAL_Delay(convTime);

	// Read value
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);

	HAL_SPI_TransmitReceive(&hspi1, &cmd_read_adc, data, (SIZE_CMD+SIZE_ADC), 500); // Instruction faster than transmit + receive

	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);

	// Convert into 32 bits value
	value = ((uint32_t)data[1]<<16) | ((uint32_t)data[2] << 8) | data[3];

	return value;
}


/**
 * @brief  Compute values to obtain compensated pressure / temperature
 * @param  _osrPr : oversampling value for pressure
 * @param  _osrPr : oversampling value for temperature
 * @retval MS56xx.P : compensated pressure in mbar
 */
double MS56xx_getPressure(ms5611_osr_t _osrPr, ms5611_osr_t _osrTemp)
{
	MS56xx.D1 = MS56xx_Read_RAW_ADC(CMD_CONV_D1, _osrPr);
	MS56xx.D2 = MS56xx_Read_RAW_ADC(CMD_CONV_D2, _osrTemp);

	// calculate 1st order temperature
	MS56xx.dT   = MS56xx.D2 - _coefficientFactory[5] * pow(2,8);
	MS56xx.TEMP = (2000 + (MS56xx.dT * _coefficientFactory[6]) / pow(2,23));

	MS56xx.OFF  = _coefficientFactory[2] * pow(2,17) + MS56xx.dT * _coefficientFactory[4] / pow(2,6);
	MS56xx.SENS = _coefficientFactory[1] * pow(2,16) + MS56xx.dT * _coefficientFactory[3] / pow(2,7);
	MS56xx.P    =(((MS56xx.D1 * MS56xx.SENS) / pow(2,21) - MS56xx.OFF) / pow(2,15));

	// calculate 2st order temperature compensation
	if(MS56xx.TEMP < 2000)
	{
		double T2     = 0.00;
		double OFF2   = 0.00;
		double SENSE2 = 0.00;

		T2     = MS56xx.dT * MS56xx.dT / pow(2,31);
		OFF2   = 61 * (MS56xx.TEMP - 2000) * (MS56xx.TEMP - 2000) / pow(2,4);
		SENSE2 = 2  * (MS56xx.TEMP - 2000) * (MS56xx.TEMP - 2000);

		if(MS56xx.TEMP < -15)
		{
			OFF2   += 15 * (MS56xx.TEMP + 1500) * (MS56xx.TEMP + 1500);
			SENSE2 += 8  * (MS56xx.TEMP + 1500) * (MS56xx.TEMP + 1500);
		}

		MS56xx.TEMP -= T2;
		MS56xx.OFF  -= OFF2;
		MS56xx.SENS -= SENSE2;

		return MS56xx.P =(((MS56xx.D1 * MS56xx.SENS) / pow(2,21) - MS56xx.OFF) / pow(2,15)) / 100;
	}
	else return MS56xx.P /= 100;
}

/**
 * @brief  Simply reading MS56xx.TEMP variable, call MS56xx_getPressure() before if you want a refresh value
 * @param  none
 * @retval value : temperature in °C
 */
double MS56xx_getTemperature(void)
{
	return MS56xx.TEMP /= 100;
}

/**
 * @brief get altitude from known sea level barometer, no pre-pressure calculation
 * @param float sea level barometer
 * @return float altitude in feet
 */
double ms56xx_getAltitude(double pressure)
{
	double altitude = 0.00;
	altitude = ((1 - (pow((pressure / SEA_PRESS), 0.190284))) * 145366.45) * FTMETERS;
	return altitude;
}
