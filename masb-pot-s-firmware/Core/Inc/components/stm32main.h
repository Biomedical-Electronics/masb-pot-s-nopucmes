/*
 * stm32main.h
 *
 *  Created on: 7 may. 2021
 *      Author: Usuario
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"
#include "components/chronoamperometry.h" // Esto aquí o en .c?

struct Handles_S {
	UART_HandleTypeDef *huart;
	ADC_HandleTypeDef *hadc;
	I2C_HandleTypeDef *hi2c;

	// Aqui añadimos los XXX_HandleTypeDef
};

// Prototypes.

void setup(struct Handles_S *handles);
void loop(void);

#endif /* INC_COMPONENTS_STM32MAIN_H_ */
