/*
 * timer.h
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#ifndef INC_COMPONENTS_TIMER_H_
#define INC_COMPONENTS_TIMER_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "components/stm32main.h"

extern TIM_HandleTypeDef htim2;

extern ADC_HandleTypeDef hadc1;

// Prototypes:
//void Timer_Config(uint32_t Sam_Per); //
void HAL_TIM_PeriodElapsedCallback(struct CA_Configuration_S caConfiguration);

#endif /* INC_COMPONENTS_TIMER_H_ */
