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

	__HAL_TIM_SET_AUTORELOAD(&htim2, (uint32_t)((caConfiguration.samplingPeriodMs*84000.0))); // Trabajamos con el timer 2 es necesario multiplicar por 84000 para llegar al número de ticks (Fórmula apuntada)
	__HAL_TIM_SET_COUNTER(&htim2, 1);

}

void Chronoamperometry_Value(struct CA_Configuration_S caConfiguration){

	uint32_t measurementTimeMs = caConfiguration.measurementTime * 1000;

	// Medida instante 0

	HAL_ADC_Start(&hadc1); // iniciamos la conversion
	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	uint32_t measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

	double vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

	HAL_ADC_Start(&hadc1); // iniciamos la conversion
	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	uint32_t measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

	double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)

	data.point=1;
	data.timeMs=0;
	data.voltage=vcell;
	data.current=icell;

	MASB_COMM_S_sendData(data);

	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(&htim2);            // Iniciamos el timer

	point_CA++;

	// Bucle while{}

	while(counter < measurementTimeMs){

		if (measureCA==TRUE){

			HAL_ADC_Start(&hadc1); // iniciamos la conversion
			HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

			measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

			vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

			HAL_ADC_Start(&hadc1); // iniciamos la conversion
			HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

			measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

			icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)

			caConfiguration = MASB_COMM_S_getCaConfiguration();

			counter = counter + caConfiguration.samplingPeriodMs;

			data.point=point_CA;
			data.timeMs=counter;
			data.voltage=vcell;
			data.current=icell;

			MASB_COMM_S_sendData(data);

			point_CA++;

			measureCA=FALSE;

		}

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
