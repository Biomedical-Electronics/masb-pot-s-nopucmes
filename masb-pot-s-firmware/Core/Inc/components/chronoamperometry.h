/**
  ******************************************************************************
  * @file		chronoamperometry.h
  * @brief		Archivo de encabezado del componente encargado de la gestión de
  *             la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#ifndef INC_COMPONENTS_CHRONOAMPEROMETRY_H_
#define INC_COMPONENTS_CHRONOAMPEROMETRY_H_

#include "stm32f4xx_hal.h"

#include "components/masb_comm_s.h"        // Necesitamos caConfiguration
#include "components/mcp4725_driver.h"     // Funcion para fijar el voltaje

#include "components/timer.h"              // header del archivo timer

// Creamos las siguientes variables:
//	En caso de ser externas se modificaran desde diversos ficheros

uint32_t counter;

extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;

uint32_t point_CA;

_Bool measureCA;

extern struct Data_S data;

struct CA_Configuration_S {

	double eDC;
	uint32_t samplingPeriodMs;
	uint32_t measurementTime;

};

// Prototypes:

void Chronoamperometry_Config(struct CA_Configuration_S caConfiguration);

void Chronoamperometry_Value(struct CA_Configuration_S caConfiguration);

#endif /* INC_COMPONENTS_CHRONOAMPEROMETRY_H_ */
