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

#include "components/masb_comm_s.h"

extern TIM_HandleTypeDef htim2;

extern ADC_HandleTypeDef hadc1;

extern uint32_t point;

extern uint32_t counter;

extern _Bool measure;

struct CA_Configuration_S caConfiguration;
struct CV_Configuration_S cvConfiguration;

// Prototypes:
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim, struct CA_Configuration_S caConfiguration, struct CV_Configuration_S cvConfiguration);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* INC_COMPONENTS_TIMER_H_ */
