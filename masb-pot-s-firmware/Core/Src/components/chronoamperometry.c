/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/chronoamperometry.h"   // header de chronoamperometry

#include "components/stm32main.h"           // Para utilizar el setup()

uint32_t point_CA = 1;    // Variable punto para la cronoamperometria

void Chronoamperometry_Config(struct CA_Configuration_S caConfiguration){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);                // Cerramos rele (EN - HIGH (1))

	float vdac = (float)(1.65-(caConfiguration.eDC/2.0));   // Formula 1

	MCP4725_SetOutputVoltage(hdac, vdac);   // Fijamos el valor de Vcell como eDC

	/*
	 * A continuación, sabiendo el tiempo de cada medicion y el tiempo total
	 * podemos saber las mediciones que tenemos que hacer, cada medicion durará
	 * (samplingPeriod). Dividimos el tiempo total (segundos) entre el tiempo
	 * de una adquisición (ms) dividido entre 1000.
	 */

	estado=CA;  // Estado para cuando queramos hacer cronoamperometria

	__HAL_TIM_SET_AUTORELOAD(&htim2, (uint32_t)((caConfiguration.samplingPeriodMs*84000.0))); // Trabajamos con el timer 2 es necesario multiplicar por 84000 para llegar al número de ticks (Fórmula apuntada)
	__HAL_TIM_SET_COUNTER(&htim2, 1);

}

void Chronoamperometry_Value(struct CA_Configuration_S caConfiguration){

	uint32_t point_CA = 1;

	uint32_t measurementTimeMs = caConfiguration.measurementTime * 1000;  // Tiempo TOTAL de la medicion

	HAL_ADC_Start(&hadc1);                     // iniciamos la conversion
	HAL_ADC_PollForConversion(&hadc1, 200);    // esperamos que finalice la conversion

	uint32_t measurement1 = HAL_ADC_GetValue(&hadc1);              // Obtenemos primer valor adc

	double vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;  // Obtenemos el valor de Vcell

	HAL_ADC_Start(&hadc1);                     // iniciamos la conversion
	HAL_ADC_PollForConversion(&hadc1, 200);    // esperamos que finalice la conversion

	uint32_t measurement2 = HAL_ADC_GetValue(&hadc1);                         //obtenemos segundo valor adc

	double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // Obtenemos el valor de Icell (todo dividido entre rtia)

	/*
	 * Enviamos el primer punto (1) que corresponderá a tiempo 0 y que tendrá
	 * los valores de Vcell y Icell calculados previamente
	 */

	data.point=point_CA;
	data.timeMs=0;
	data.voltage=vcell;
	data.current=icell;

	MASB_COMM_S_sendData(data);        // Enviamos los datos

	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

	HAL_TIM_Base_Start_IT(&htim2);     // Iniciamos el timer

	//Sumamos un punto al contador point_CA

	point_CA++;

	// Bucle while hasta que el contador (counter) supera al tiempo global de medida (measurementTimeMs)

	while(counter < measurementTimeMs){

		if (measureCA==TRUE){    // En caso de que esta variable sea TRUE y por tanto el timer ha llegado a un periodo:

			HAL_ADC_Start(&hadc1);  // iniciamos la conversion
			HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

			measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

			vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;             // Obtenemos el valor de Vcell

			HAL_ADC_Start(&hadc1); // iniciamos la conversion
			HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

			measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

			icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // Obtenemos el valor de Icell (todo dividido entre rtia)

			caConfiguration = MASB_COMM_S_getCaConfiguration();

			/*
			 * Enviamos el siguiente punto point_CA que corresponderá al tiempo counter y que tendrá
			 * los valores de Vcell y Icell calculados previamente
			 */

			counter = counter + caConfiguration.samplingPeriodMs;   // sumamos un periodo a la variable counter

			data.point=point_CA;    // Punto de la medición que estamos enviando
			data.timeMs=counter;    // Ms que llevamos desde el inicio de la conversión
			data.voltage=vcell;     // Valor de Vcell calculado
			data.current=icell;     // Valor de Icell calculado

			MASB_COMM_S_sendData(data);   // Enviamos los datos

			point_CA++;   // Sumamos un punto al contador

			/*
			 * Una vez hemos enviado los puntos e incrementaod point_CA ponemos la variable measureCV a
			 * FALSE para evitar que entre en el bucle de nuevo. Esta variable solo volverá a ser TRUE
			 * (y por tanto entraremos en el bucle) si se activa la función callback del timer
			 */

			measureCA=FALSE;   // Volvemos a poner la variable bandera como FALSE

		}

	}

	HAL_TIM_Base_Stop_IT(&htim2);             // Detenemos el timer al finalizar la medición

	HAL_ADC_Stop(&hadc1);					  // Paramos la conversion ADC

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

	__HAL_TIM_SET_COUNTER(&htim2, 0);         // Reiniciamos el contador del timer a 0

	// Reiniciamos variables a los valores iniciales

	estado = IDLE;
	point_CA = 1;
	counter = 0;
}
