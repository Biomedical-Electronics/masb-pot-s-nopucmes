/*
 * stm32main.h
 *
 *  Created on: 7 may. 2021
 *      Author: Usuario
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"

#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"
#include "components/chronoamperometry.h"
#include "components/cyclic_voltammetry.h"

extern uint16_t estado;

extern MCP4725_Handle_T hdac;

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
