/*
 * timer.c
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#include "components/timer.h"

#include "components/stm32main.h"

uint32_t point = 1; // variable punto que enviemos al host

uint32_t counter = 0;

uint32_t measurement1 = 0;

uint32_t measurement2 = 0;


//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim, struct CA_Configuration_S caConfiguration, struct CV_Configuration_S cvConfiguration)

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	__NOP();

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion

	measurement1 = HAL_ADC_GetValue(&hadc1);

	double vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion

	measurement2 = HAL_ADC_GetValue(&hadc1);

	double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)

	//int32_t icell = ((measurement2 - 1.65)*2)/10000;

	if (estado == CA){

		caConfiguration = MASB_COMM_S_getCaConfiguration();

		counter = counter + caConfiguration.samplingPeriodMs;

		// struct Data_S data;

		data.point=point;
		data.timeMs=counter;
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_sendData(data);

		__NOP();

	}

	else{
		cvConfiguration = MASB_COMM_S_getCvConfiguration();

		struct Data_S data;

		data.point=point;
		data.timeMs=point*(cvConfiguration.eStep/cvConfiguration.scanRate);
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_sendData(data);

		measureCV = TRUE;    // ESTO ESTÁ BIEN ASÍ

		__NOP();
	}

	point++;

}
