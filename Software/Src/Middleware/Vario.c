/*
 * Vario.c
 *
 *  Created on: 10 févr. 2019
 *      Author: jo
 */
#include "Middleware/Macro.h"
#include "Middleware/Vario.h"
#include "Peripherals/MS56xx.h"
#include "Peripherals/UserButtons.h"
#include "stdio.h"
#include "usart.h"

// sensibility setting
#define NB_OF_SAMPLE  10    // Number of sample to filter sensor
#define COEF_FAST     0.09  // Coefficient for filter low past fast
#define COEF_SLOW     0.06  // Coefficient for filter
#define COEF_LOWPASS  0.12  // Coefficient for low pass tone
#define MAX_SENS      40    // Max trigger threshold for climb
#define MIN_SENS      10    // Min trigger threshold for climb
#define MIN_FALL      -30   // Min trigger threshold for fall
#define STEP_SENS     10    // Step to set threshold

double data[NB_OF_SAMPLE] = {0};

void welcome(void)
{
	/* Hyperterminal configuration:
	 * - Data Length = 7 Bits
	 * - One Stop Bit
	 * - Odd parity
	 * - BaudRate = 115200 baud
	 * - Flow control: None */

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, LED_ON);

	printf("\n\r*** Vario *** \n\r");
	printf("Version : 0.1\n");
	printf("Date    : 27/01/2019\n");
	printf("uC      : STM32L071KBU\n");
	printf("Sensor  : MS5607\n");
	printf("\r__Program START !__ \n\r");
	MS56xx_Init();

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, LED_OFF);

	playWelcome();
}

void playWelcome(void)
{

}

//uint8_t vario(void)
//{
//	static int16_t ddsAcc;
//	static double pressure, toneFreq, toneFreqLowpass, lowPassFast, lowPassSlow;
//
//	// Value reading
//	//pressure = MS5611_Read_RAW_pressure(MS5611_STANDARD);
//
//	// DEBUG
//	// printf("pressure: \n");
//
//	// DEBUG
//	// Serial.println("-----------------------------");
//	// Serial.print("pressure: "); Serial.println(pressure,4);
//
//	// Filtering on two levels
//	lowPassFast = lowPassFast + (pressure - lowPassFast) * COEF_FAST;
//	lowPassSlow = lowPassSlow + (pressure - lowPassSlow) * COEF_SLOW;
//
//	// DEBUG
//	// Serial.print("lowPassFast : "); Serial.println(lowPassFast,4);
//	// Serial.print("lowPassSlow : "); Serial.println(lowPassSlow,4);
//
//	// Make difference
//	toneFreq = (lowPassSlow - lowPassFast) * 50;
//
//	// DEBUG
//	// Serial.print("toneFreq: "); Serial.println(toneFreq,4);
//
//	// New filtering
//	toneFreqLowpass = toneFreqLowpass + (toneFreq - toneFreqLowpass) * COEF_LOWPASS;
//
//	// DEBUG
//	// Serial.print("toneFreqLowpass: "); Serial.println(toneFreqLowpass,4);
//
//	// Value is constrain to stay in audible frequency
//	//toneFreq = constrain(toneFreqLowpass, -400, 500);
//
//	// DEBUG
//	// Serial.print("toneFreq: "); Serial.println(toneFreq,4);
//
//	// "ddsAcc" give a "delay time" to produce a bip-bip-bip....
//	ddsAcc += toneFreq * 100 + 500;
//
//	// DEBUG
//	// Serial.print("ddsAcc: "); Serial.println(ddsAcc);
//
//	// Play tone depending variation.
//	return 1;//bipSound(toneFreq, ddsAcc);
//}

uint8_t vario(void)
{
	static uint8_t sample = 0;

	//static uint32_t start = 0;

	if(sample < NB_OF_SAMPLE)
	{
		data[sample] = MS56xx_getPressure(OSR_STANDARD, OSR_LOW_POWER);
		sample++;
	}
	else
	{
		double value = 0.00;
		double valueTmp = 0.00;
		sample = 0;

		for(uint8_t i; i<NB_OF_SAMPLE; i++)
		{
			valueTmp+=data[i];
		}
		value = valueTmp / NB_OF_SAMPLE;


		//value = filter(data);

		 printf("%.2f\n",ms56xx_getAltitude(value));
	}

	return data[1];
}
