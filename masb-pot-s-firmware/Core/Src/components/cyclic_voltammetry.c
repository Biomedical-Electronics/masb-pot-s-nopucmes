/**
  ******************************************************************************
  * @file		cyclic_voltammetry.c
  * @brief		Gestión de la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/cyclic_voltammetry.h"

extern TIM_HandleTypeDef htim2;

void Voltammetry_Config(struct CV_Configuration_S cvConfiguration){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);                // Cerramos rele (EN - HIGH (1))

	float vdac = (float)(1.65-(cvConfiguration.eBegin/2.0));   // Formula 1

	MCP4725_SetOutputVoltage(hdac, vdac);    // Fijamos el valor de Vcell a eBegin

	estado = CV;

	__HAL_TIM_SET_AUTORELOAD(&htim2, cvConfiguration.eStep/cvConfiguration.scanRate); // sampling period defined by eStep/scanRate

	HAL_TIM_Base_Start_IT(&htim2);           // Iniciamos el timer

}

void Voltammetry_Value(struct CV_Configuration_S cvConfiguration){

	uint32_t cycles = 0; // we start at 0, when a cycle it has been done we will add 1 to this variable and get out of the loop

	_Bool measure = FALSE;

	double vcell=cvConfiguration.eBegin;

	double vobj=cvConfiguration.eVertex1;   // Igualamos VObjetivo a eVertex1

	while(cycles < cvConfiguration.cycles){

		if (measure){
			measure=FALSE;

			if (vcell==vobj){

				if (vobj==cvConfiguration.eVertex1){
					vobj=cvConfiguration.eVertex2;

				} else {
					if (vobj==cvConfiguration.eVertex2){
						vobj=cvConfiguration.eBegin;

					} else {
						vobj=cvConfiguration.eVertex1;
						cycles+=1; // when vobj equals eBegin, means a cycle has been done, if we add one to cycles count,
						// and the total num of cycles == cvConfiguration.cycles, we will get out of the loop
					}
				}


			} else {

				if (vcell+cvConfiguration.eStep>vobj){
					vcell=vobj;

					float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

					MCP4725_SetOutputVoltage(hdac, vdac); // administrem el voltatge al Working Electrode

				} else {

					vcell+=((vobj-vcell)/abs(vobj-vcell))*cvConfiguration.eStep; // incrementem eStep a Vcell

					float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

					MCP4725_SetOutputVoltage(hdac, vdac); // administrem el nou voltatge al Working Electrode

				}

			}

		} else {

		}

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

	__HAL_TIM_SET_COUNTER(&htim2, 0);         // Reiniciamos el contador del timer a 0

	HAL_TIM_Base_Stop_IT(&htim2);             // Detenemos el timer al finalizar la medición

	estado = IDLE;

	point = 0;

	}

}
