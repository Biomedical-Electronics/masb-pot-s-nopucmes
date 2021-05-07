/*
 * stm32main.h
 *
 *  Created on: 7 may. 2021
 *      Author: Usuario
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"

struct Handles_S {
	UART_HandleTypeDef *huart;
	// Aqui añadimos los XXX_HandleTypeDef
};

// Prototypes.

void setup(struct Handles_S *handles);
void loop(void);

#endif /* INC_COMPONENTS_STM32MAIN_H_ */
