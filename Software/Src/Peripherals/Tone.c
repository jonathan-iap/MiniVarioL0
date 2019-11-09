/*
 * Tone.c
 *
 *  Created on: 10 févr. 2019
 *      Author: jo
 */

#include "Peripherals/Tone.h"
#include "tim.h"

void tone(uint16_t note, uint16_t duration, uint8_t volume)
{
	setToneVolume(volume);

	if(volume != 0)
	{
		setTonePeriode(note);

		toneStart();

		HAL_Delay(duration);
	}

	toneStop();
}


void setTonePeriode(uint16_t note)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	uint32_t periode = 0;

	periode = (uint32_t) (SystemCoreClock / (note*2));

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = periode;
	htim3.Init.ClockDivision = 0;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	HAL_TIM_OC_Init(&htim3);

	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
}


void setToneVolume(uint8_t volume)
{
	switch (volume)
	{
	case VOLUME_LOW:
		HAL_GPIO_WritePin(VOL_EN1_GPIO_Port, VOL_EN1_Pin, 0);
		HAL_GPIO_WritePin(VOL_EN2_GPIO_Port, VOL_EN2_Pin, 1);
		break;
	case VOLUME_MEDIUM:
		HAL_GPIO_WritePin(VOL_EN1_GPIO_Port, VOL_EN1_Pin, 1);
		HAL_GPIO_WritePin(VOL_EN2_GPIO_Port, VOL_EN2_Pin, 0);
		break;
	case VOLUME_HIGH:
		HAL_GPIO_WritePin(VOL_EN1_GPIO_Port, VOL_EN1_Pin, 1);
		HAL_GPIO_WritePin(VOL_EN2_GPIO_Port, VOL_EN2_Pin, 1);
		break;
	case VOLUME_MUTE:
		HAL_GPIO_WritePin(VOL_EN1_GPIO_Port, VOL_EN1_Pin, 0);
		HAL_GPIO_WritePin(VOL_EN2_GPIO_Port, VOL_EN2_Pin, 0);
		break;
	default:
		break;
	}
}


void toneStart(void)
{
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);
}

void toneStop(void)
{
	HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_1);
}
