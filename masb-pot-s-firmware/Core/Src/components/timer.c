/*
 * timer.c
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#include "components/timer.h"

#include "components/stm32main.h"

uint32_t point_CA = 0;    // variable punto que enviemos al host para CA

uint32_t point_CV = 1;    // variable punto que enviemos al host

uint32_t counter = 0;  // variable que controla el tiempo

uint32_t measurement1 = 0;

uint32_t measurement2 = 0;

_Bool inicio=TRUE;//variable que controla si es el primer punto enviado

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim, struct CA_Configuration_S caConfiguration, struct CV_Configuration_S cvConfiguration)

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

	double vcell=(1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

	double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)


	if (estado == CA){ //si pedimos cronoamperometria

		if (inicio){ // mira si es el primer punto y lo envia con valores iniciales

			data.point=point_CA;
			data.timeMs=counter;
			data.voltage=0;
			data.current=0;

			MASB_COMM_S_sendData(data);
			inicio=FALSE;

			point_CA++;

		}else{ //si no es el primer punto

			caConfiguration = MASB_COMM_S_getCaConfiguration();

			counter = counter + caConfiguration.samplingPeriodMs;

			data.point=point_CA;
			data.timeMs=counter;
			data.voltage=vcell;
			data.current=icell;

			MASB_COMM_S_sendData(data);

			point_CA++;

		}
	}


	else{      //si pedimos cronoamperometria

			counter = counter + ts*1000;

			data.point=point_CV;
			data.timeMs=counter;
			data.voltage=vcell;
			data.current=icell;

			MASB_COMM_S_sendData(data);

			measureCV = TRUE;

			__NOP();

			point_CV++;

	}

}
