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

extern uint32_t point_CA;

extern uint32_t point_CV;

extern _Bool inicio;

extern uint32_t counter;

extern _Bool measureCV;

extern _Bool measureCA;

struct Data_S data;

struct CA_Configuration_S caConfiguration;
struct CV_Configuration_S cvConfiguration;

// Prototypes:
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim, struct CA_Configuration_S caConfiguration, struct CV_Configuration_S cvConfiguration);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* INC_COMPONENTS_TIMER_H_ */
