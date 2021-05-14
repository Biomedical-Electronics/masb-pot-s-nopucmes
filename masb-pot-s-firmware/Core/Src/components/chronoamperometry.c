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

extern TIM_HandleTypeDef htim2;

void Chronoamperometry_Value(struct CA_Configuration_S caConfiguration){

	double eDC = caConfiguration.eDC;

	Voltage2DAC(eDC);   // Fijamos el valor de Vcell como eDC

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);  // Cerramos rele (EN - HIGH (1))

	uint32_t sampling_P = caConfiguration.samplingPeriodMs;
	uint32_t measurement_T = caConfiguration.measurementTime;

	// A continuación, sabiendo el tiempo de cada medicion y el tiempo total
	// podemos saber las mediciones que tenemos que hacer, cada medicion durará
	// (samplingPeriod). Dividimos el tiempo total (segundos) entre el tiempo
	// de una adquisición (ms) dividido entre 1000.

	uint16_t Num_Measurements = ((measurement_T/(sampling_P/1000))* 4096.0f);

	uint16_t measurements = 0;

	timerConfiguration(sampling_P);

	// Loop mientras el numero de medidas sea menor al total

	while (measurements <= Num_Measurements){

		HAL_TIM_Base_Start(&htim2); // iniciamos timer

		if (measurements == Num_Measurements){
			// Estado = "IDLE"; (?)

		}

		// Deberiamos recibir los datos caudno el timer sea igual a sampling_P

		HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);

		// Sumamos una medida:

		measurements++;

	}

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

}
