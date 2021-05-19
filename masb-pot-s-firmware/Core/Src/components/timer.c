/*
 * timer.c
 *
 *  Created on: 14 may. 2021
 *      Author: Usuario
 */

#include "components/timer.h"
/*
void Timer_Config(uint32_t Sam_Per){

	// Calculamos el número de ticks que queremos, este
	// dependerá de Sam_per (parámetro de entrada)

	uint32_t ticks = (84e6)/(Sam_Per/1000);

	htim2.Instance = TIM2;
		htim2.Init.Period = ticks;
		htim2.Init.Prescaler = 0;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

}
*/
extern uint32_t point=0; //variable punto que enviemos al host

void HAL_TIM_PeriodElapsedCallback(struct CA_Configuration_S caConfiguration, struct CV_Configuration_S cvConfiguration){

	HAL_ADC_Start(&hadc1); // iniciamos la conversion
	uint32_t measurement1=HAL_ADC_GetValue (&hadc1);
	uint32_t measurement2=HAL_ADC_GetValue (&hadc1);

	double vcell=(1.65-measurement1*3.3/(1023))*2;//formula 2
	double icell=((1.65-measurement2*3.3/(1023))*2)/10000;//formula 3 (dividido rtia)

	point++;

	if (estado==CA){
		counter+=caConfiguration.samplingPeriodMs;

		struct Data_S data;

		data.point=point;
		data.timeMs=point*caConfiguration.samplingPeriodMs;
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_SendData(data);

	}else{

		struct Data_S data;
		data.point=point;
		data.timeMs=point*(cvConfiguration.eStep/cvConfiguration.scanRate);
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_SendData(data);

		measure=TRUE;
	}

}
