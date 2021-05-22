/*
 * timer.c
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#include "components/timer.h"

#include "components/stm32main.h"

uint32_t point = 0; // variable punto que enviemos al host

uint32_t counter = 0;

uint32_t measurement1 = 0;

uint32_t measurement2 = 0;

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim, struct CA_Configuration_S caConfiguration, struct CV_Configuration_S cvConfiguration)

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	__NOP();

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	// HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion

	measurement1 = HAL_ADC_GetValue(&hadc1);

	// HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion

	measurement2 = HAL_ADC_GetValue(&hadc1);

	uint32_t vcell=(1.65-measurement1*3.3/(1023))*2;          // formula 2

	uint32_t icell=((1.65-measurement2*3.3/(1023))*2)/10000;  // formula 3 (dividido rtia)

	point++;

	if (estado == CA){

		caConfiguration = MASB_COMM_S_getCaConfiguration();

		counter = counter + (caConfiguration.samplingPeriodMs)/1000;

		struct Data_S data;

		data.point=point;
		data.timeMs=point*(caConfiguration.samplingPeriodMs);
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_sendData(data);

		__NOP();

	}

	/*else{
		cvConfiguration = MASB_COMM_S_getCvConfiguration();
		struct Data_S data;
		data.point=point;
		data.timeMs=point*(cvConfiguration.eStep/cvConfiguration.scanRate);
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_sendData(data);

		measure = TRUE;
	}*/

}
