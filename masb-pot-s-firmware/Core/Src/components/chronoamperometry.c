/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/chronoamperometry.h"  // header de chronoamperometry


extern TIM_HandleTypeDef htim2;

void Chronoamperometry_Config(struct CA_Configuration_S caConfiguration){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);                // Cerramos rele (EN - HIGH (1))

	float vdac = (float)(1.65-(caConfiguration.eDC/2.0));   // Formula 1

	MCP4725_SetOutputVoltage(hdac, vdac);   // Fijamos el valor de Vcell como eDC

	// A continuación, sabiendo el tiempo de cada medicion y el tiempo total
	// podemos saber las mediciones que tenemos que hacer, cada medicion durará
	// (samplingPeriod). Dividimos el tiempo total (segundos) entre el tiempo
	// de una adquisición (ms) dividido entre 1000.

	__HAL_TIM_SET_AUTORELOAD(&htim2, caConfiguration.samplingPeriodMs);

	HAL_TIM_Base_Start_IT(&htim2);            // Iniciamos el timer

}

void Chronoamperometry_Value(struct CA_Configuration_S caConfiguration){

	uint32_t counter = 0;

	while(counter <= caConfiguration.measurementTime){}

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

	__HAL_TIM_SET_COUNTER(&htim2, 0);         // Reiniciamos el contador del timer a 0

	HAL_TIM_Base_Stop_IT(&htim2);             // Detenemos el timer al finalizar la medición


}
