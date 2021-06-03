/*
 * timer.c
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#include "components/timer.h"

#include "components/stm32main.h"

uint32_t counter = 0;  // variable que controla el tiempo

uint32_t measurement1 = 0;

uint32_t measurement2 = 0;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

/*  ¡¡QUITAR!!

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

	double vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

	double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)

*/

	/*
	 * En función de la variable estado entramos en la cronoamperometria (estado==CA) o a la voltametria
	 * cíclica (estado==CV)
	 */

	if (estado == CA){  // si pedimos cronoamperometria

		measureCA = TRUE;   // Cambiamos el estado de esta variable para realizar la medición de la cronoamperometria


		/* ¡¡QUITAR!!

			caConfiguration = MASB_COMM_S_getCaConfiguration();

			counter = counter + caConfiguration.samplingPeriodMs;

			data.point=point_CA;
			data.timeMs=counter;
			data.voltage=vcell;
			data.current=icell;

			MASB_COMM_S_sendData(data);

			point_CA++;
			*/

	}
	else{               // si pedimos voltametria ciclica

		measureCV = TRUE;   // Cambiamos el estado de esta variable para realizar la medición de la voltametria

		/*HAL_ADC_Start(&hadc1); // iniciamos la conversion
		HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

		measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

		double vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

		HAL_ADC_Start(&hadc1); // iniciamos la conversion
		HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

		measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

		double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)

		counter = counter + ts*1000;

		data.point=point_CV;
		data.timeMs=counter;
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_sendData(data);*/

		__NOP();

	}

}
