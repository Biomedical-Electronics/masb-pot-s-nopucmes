/*
 * timer.c
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#include "components/timer.h"

#include "components/stm32main.h"

uint32_t counter = 0;  // variable que controla el tiempo

uint32_t measurement1 = 0;

uint32_t measurement2 = 0;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	/*
	 * En función de la variable estado entramos en la cronoamperometria (estado==CA) o a la voltametria
	 * cíclica (estado==CV)
	 */

	if (estado == CA){  // si pedimos cronoamperometria

		measureCA = TRUE;   // Cambiamos el estado de esta variable para realizar la medición de la cronoamperometria

	}
	else{               // si pedimos voltametria ciclica

		measureCV = TRUE;   // Cambiamos el estado de esta variable para realizar la medición de la voltametria

	}

}
