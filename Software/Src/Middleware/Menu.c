/*
 * Menu.c
 *
 *  Created on: 28 août 2019
 *      Author: jo
 */

#include "Middleware/Menu.h"
#include "Middleware/Macro.h"
#include "Middleware/Vario.h"
#include "Peripherals/Power.h"
#include "Peripherals/UserButtons.h"
#include "Peripherals/Tone.h"

#define EXTINCTION_TIME 3000

void varioRun(void)
{
	vario();

	uint16_t btn = getButtons();

	if( btn!= 0)
	{
		menu(btn);
	}

}

void menu(uint16_t _btn)
{
	static uint8_t volume = 1;

	switch (_btn)
	{
	case BTN_PWR_ON_OFF_Pin:
		selectAction();
		break;
	case BTN_UP_Pin:
		volumeUp(&volume);
		break;
	case BTN_DOWN_Pin:
		volumeDown(&volume);
		break;
	default:
		break;
	}
}

void selectAction(void)
{
	uint32_t start_T = HAL_GetTick();
	uint16_t btn = BTN_PWR_ON_OFF_Pin;

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, LED_ON);

	while(btn == BTN_PWR_ON_OFF_Pin ||
			btn == (BTN_PWR_ON_OFF_Pin + BTN_UP_Pin) ||
			btn == (BTN_PWR_ON_OFF_Pin + BTN_DOWN_Pin))
	{
		switch (btn)
		{
		case BTN_PWR_ON_OFF_Pin :
			if((HAL_GetTick()-start_T) >= EXTINCTION_TIME)
			{
				while(getButtons() == BTN_PWR_ON_OFF_Pin)
				{
					HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
					HAL_Delay(100);
				}

				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, LED_OFF);
				HAL_Delay(1000);
				powerOff();
			}
			break;
		case BTN_PWR_ON_OFF_Pin + BTN_UP_Pin :
		sensitivityUp();
		start_T = HAL_GetTick();
		break;
		case BTN_PWR_ON_OFF_Pin + BTN_DOWN_Pin :
		sensitivityDown();
		start_T = HAL_GetTick();
		break;
		default:
			break;
		}

		btn = getButtons();
	}

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, LED_OFF);
}

void volumeUp(uint8_t *_vol)
{
	if(*_vol < VOLUME_HIGH)
	{
		*_vol+=1;
		setToneVolume(*_vol);
		tone(NOTE_A4, 100, *_vol);
	}
	else
	{
		tone(NOTE_A3, 300, *_vol);
	}

	HAL_Delay(500);
}


void volumeDown(uint8_t *_vol)
{
	if(*_vol > VOLUME_LOW)
	{
		*_vol-=1;
		setToneVolume(*_vol);
		tone(NOTE_A4, 100, *_vol);
	}
	else
	{
		tone(NOTE_A3, 300, *_vol);
	}

	HAL_Delay(500);
}


void sensitivityUp()
{

}

void sensitivityDown()
{

}
