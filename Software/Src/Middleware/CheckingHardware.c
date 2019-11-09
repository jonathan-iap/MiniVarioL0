/*
 * CheckingHardware.c
 *
 *  Created on: 23 août 2019
 *      Author: jo
 */

#include "Middleware/Macro.h"
#include "Middleware/CheckingHardware.h"
#include "Middleware/Vario.h"
#include "Peripherals/MS56xx.h"
#include "Peripherals/Tone.h"
#include "Peripherals/UserButtons.h"
#include "usart.h"
#include "stdio.h"

void checkingHardware(void)
{
	welcome(); // Print informations on the serial port and led blink

	MS56xx_Init(); // Check sensor;

	// Play tone at different volume at 400 Hz
	tone(400, 1000, VOLUME_LOW);
	HAL_Delay(100);
	tone(400, 1000, VOLUME_MEDIUM);
	HAL_Delay(100);
	tone(400, 1000, VOLUME_HIGH);

	// Check buttons
	printf("Press button UP\n");
	while(getButtons() != BTN_UP_Pin);
	printf("Press button DOWN\n");
	while(getButtons() != BTN_DOWN_Pin);
	printf("Press button PWR\n");
	while(getButtons() != BTN_PWR_ON_OFF_Pin);
	printf("Press button PWR + UP\n");
	while(getButtons() != (BTN_PWR_ON_OFF_Pin+BTN_UP_Pin));
	printf("Press button PWR + DOWN\n");
	while(getButtons() != (BTN_PWR_ON_OFF_Pin+BTN_DOWN_Pin));

	printf("Successful check done great job !\n");
}
