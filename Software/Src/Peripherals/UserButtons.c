/*
 * UserButtons.c
 *
 *  Created on: 10 févr. 2019
 *      Author: jo
 */
#include "Peripherals/UserButtons.h"


uint16_t getButtons(void)
{
  uint16_t btnState = 0;

  // Read buttons
  if( !HAL_GPIO_ReadPin(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin))
  {
	  btnState += BTN_DOWN_Pin;
  }

  if( !HAL_GPIO_ReadPin(BTN_UP_GPIO_Port, BTN_UP_Pin))
  {
	  btnState += BTN_UP_Pin;
  }

  if( HAL_GPIO_ReadPin(BTN_PWR_ON_OFF_GPIO_Port, BTN_PWR_ON_OFF_Pin))
  {
	  btnState += BTN_PWR_ON_OFF_Pin;
  }

  // Return an unique value
  return btnState;
}
