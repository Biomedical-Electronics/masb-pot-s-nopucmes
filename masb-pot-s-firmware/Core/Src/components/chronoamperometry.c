/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/chronoamperometry.h"  // header de chronoamperometry
#include "components/masb_comm_s.h"        // Necesitamos caConfiguration
//#include "components/mcp4725_drive.h"      // Funcion para fijar el voltaje
//#include "components/timer.h"              // header del archivo timer
//#include "components/stm32main.h"          // Para utilizar el setup()

extern TIM_HandleTypeDef htim2;

void Chronoamperometry_Config(struct CA_Configuration_S caConfiguration){

	double eDC = caConfiguration.eDC;

	MCP4725_SetOutputVoltage(hdac, eDC);   // Fijamos el valor de Vcell como eDC

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);  // Cerramos rele (EN - HIGH (1))

	uint32_t sampling_P = caConfiguration.samplingPeriodMs;
	uint32_t measurement_T = caConfiguration.measurementTime;

	// A continuación, sabiendo el tiempo de cada medicion y el tiempo total
	// podemos saber las mediciones que tenemos que hacer, cada medicion durará
	// (samplingPeriod). Dividimos el tiempo total (segundos) entre el tiempo
	// de una adquisición (ms) dividido entre 1000.

	uint16_t Max_Measurements = ((measurement_T/(sampling_P/1000)) * 4096.0f);

	uint16_t Num_Measurement = 0;

	uint8_t Estado = CA;

	timerConfiguration(sampling_P); // Configuramos el timer con el periodo etc

	HAL_TIM_Base_Start(&htim2);          // Iniciamos el timer

}

void Chronoamperometry_Value(Num_Measurement, Max_Measurements){

	// HACES LA MEDIDA A LOS X MS

	Num_Measurement++;

	if (Num_Measurement == Max_Measurements){
		HAL_TIM_Base_Stop(&htim2);
		uint16_t Estado = IDLE;  // (?)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))
	}

}

