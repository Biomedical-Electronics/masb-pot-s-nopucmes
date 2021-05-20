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
#include "components/stm32main.h"          // Para utilizar el setup()
#include "components/timer.h"              // header del archivo timer


struct CA_Configuration_S {

	double eDC;
	uint32_t samplingPeriodMs;
	uint32_t measurementTime;

};

// Prototypes:

void Chronoamperometry_Config(struct CA_Configuration_S caConfiguration);

void Chronoamperometry_Value(struct CA_Configuration_S caConfiguration);

#endif /* INC_COMPONENTS_CHRONOAMPEROMETRY_H_ */
