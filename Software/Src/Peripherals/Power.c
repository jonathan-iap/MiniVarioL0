/*
 * Power.c
 *
 *  Created on: Aug 27, 2019
 *      Author: jo
 */

#include "Peripherals/Power.h"
#include "Peripherals/UserButtons.h"

#define PRESS_TIME_ON 3000

uint8_t powerOn(void)
{
	uint32_t start_T   = HAL_GetTick();

	while(getButtons() == BTN_PWR_ON_OFF_Pin)
	{
		if((HAL_GetTick()-start_T) >= PRESS_TIME_ON)
		{
			HAL_GPIO_WritePin(PWR_LATCH_GPIO_Port, PWR_LATCH_Pin, 1);

			return HAL_OK;
		}
	}

	HAL_GPIO_WritePin(PWR_LATCH_GPIO_Port, PWR_LATCH_Pin, 0);

	return HAL_ERROR;
}


void powerOff(void)
{
	HAL_GPIO_WritePin(PWR_LATCH_GPIO_Port, PWR_LATCH_Pin, 0);
}
