/*
 * timer.h
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#ifndef INC_COMPONENTS_TIMER_H_
#define INC_COMPONENTS_TIMER_H_

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim2;

// Prototypes:
void Timer_Config(uint32_t Sam_Per); //

#endif /* INC_COMPONENTS_TIMER_H_ */
