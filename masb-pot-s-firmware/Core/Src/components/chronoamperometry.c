/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/chronoamperometry.h"  // header de chronoamperometry

#include "components/stm32main.h"          // Para utilizar el setup()

void Chronoamperometry_Config(struct CA_Configuration_S caConfiguration){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);                // Cerramos rele (EN - HIGH (1))

	float vdac = (float)(1.65-(caConfiguration.eDC/2.0));   // Formula 1

	MCP4725_SetOutputVoltage(hdac, vdac);   // Fijamos el valor de Vcell como eDC

	// A continuación, sabiendo el tiempo de cada medicion y el tiempo total
	// podemos saber las mediciones que tenemos que hacer, cada medicion durará
	// (samplingPeriod). Dividimos el tiempo total (segundos) entre el tiempo
	// de una adquisición (ms) dividido entre 1000.

	estado=CA; //Estado para cuando queramos hacer cronoamperometria

	__HAL_TIM_SET_AUTORELOAD(&htim2, (caConfiguration.samplingPeriodMs)*84000); // Trabajamos con el timer 2 es necesario multiplicar por 84000 para llegar al número de ticks (Fórmula apuntada)

}

void Chronoamperometry_Value(struct CA_Configuration_S caConfiguration){

	HAL_TIM_Base_Start_IT(&htim2);            // Iniciamos el timer

	// uint32_t MT = caConfiguration.measurementTime;
	uint32_t measurementTimeMs = caConfiguration.measurementTime * 1000;

	// medida instante 0


	// struct Data_S data;


	while(counter < measurementTimeMs){

	}

	HAL_TIM_Base_Stop_IT(&htim2);             // Detenemos el timer al finalizar la medición

	HAL_ADC_Stop(&hadc1);					  //Paramos conversion adc

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

	__HAL_TIM_SET_COUNTER(&htim2, 0);         // Reiniciamos el contador del timer a 0

	estado = IDLE;                            // Reiniciamos variables
	point_CA = 1;
	counter = 0;
	inicio= TRUE;
}
