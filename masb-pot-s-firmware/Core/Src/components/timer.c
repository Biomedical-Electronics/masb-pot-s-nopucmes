/*
 * timer.c
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#include "components/timer.h"

void Timer_Config(uint32_t Sam_Per){

	// Calculamos el número de ticks que queremos, este
	// dependerá de Sam_per (parámetro de entrada)

	uint32_t ticks = (84e6)/(Sam_Per/1000);

	htim2.Instance = TIM2;
		htim2.Init.Period = ticks;
		htim2.Init.Prescaler = 0;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

}
