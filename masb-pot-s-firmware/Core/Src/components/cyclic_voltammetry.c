/**
  ******************************************************************************
  * @file		cyclic_voltammetry.c
  * @brief		Gestión de la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/cyclic_voltammetry.h"

#include "components/stm32main.h"          // Para utilizar el setup()‍

double ts = 0;

void Voltammetry_Config(struct CV_Configuration_S cvConfiguration){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);                // Cerramos rele (EN - HIGH (1))

	float vdac = (float)(1.65-(cvConfiguration.eBegin/2.0));   // Formula 1

	MCP4725_SetOutputVoltage(hdac, vdac);    // Fijamos el valor de Vcell a eBegin

	estado = CV;

	__HAL_TIM_SET_AUTORELOAD(&htim2, (cvConfiguration.eStep/cvConfiguration.scanRate*1000)*84000); // sampling period defined by eStep/scanRate

}

void Voltammetry_Value(struct CV_Configuration_S cvConfiguration){

	__NOP();

	HAL_TIM_Base_Start_IT(&htim2);           // Iniciamos el timer

	uint32_t cycles = 0; // we start at 0, when a cycle it has been done we will add 1 to this variable and get out of the loop

	measureCV = FALSE;

	double vcell = cvConfiguration.eBegin;

	double vobj = cvConfiguration.eVertex1;   // Igualamos VObjetivo a eVertex1

	ts = cvConfiguration.eStep/cvConfiguration.scanRate;

	while(cycles < cvConfiguration.cycles){

		if (measureCV==TRUE){

			__NOP();

			if (vcell==vobj){

				__NOP();

				if (vobj==cvConfiguration.eVertex1){
					vobj=cvConfiguration.eVertex2;

					__NOP();

				} else{
					if (vobj==cvConfiguration.eVertex2){
						__NOP();

						vobj=cvConfiguration.eBegin;

					} else {
						vobj=cvConfiguration.eVertex1;
						cycles += 1;   // when vobj equals eBegin, means a cycle has been done, if we add one to cycles count,
						               // and the total num of cycles == cvConfiguration.cycles, we will get out of the loop
						__NOP();
					}
				}

			}

			if (vcell==cvConfiguration.eVertex1){

				vcell = vcell + ((vobj-vcell)/(ABS(vobj-vcell)))*cvConfiguration.eStep; // incrementem eStep a Vcell

				float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

				MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el nou voltatge al Working Electrode

				__NOP();

			}

			if (vcell==cvConfiguration.eVertex2){

				vcell = vcell + ((vobj-vcell)/(ABS(vobj-vcell)))*cvConfiguration.eStep; // incrementem eStep a Vcell

				float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

				MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el nou voltatge al Working Electrode

				__NOP();

			}

			else {

				if (ABS(vcell+cvConfiguration.eStep) > ABS(vobj)){  // ¡¡Esto hay que mirarlo!!

					if (vobj==cvConfiguration.eBegin){

						vcell = vcell + ((vobj-vcell)/(ABS(vobj-vcell)))*cvConfiguration.eStep; // incrementem eStep a Vcell

						float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

						MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el nou voltatge al Working Electrode

					}

					else{

						vcell=vobj;

						float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

						MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el voltatge al Working Electrode

						__NOP();

					}

				} else {

					__NOP();

					vcell = vcell + ((vobj-vcell)/(ABS(vobj-vcell)))*cvConfiguration.eStep; // incrementem eStep a Vcell

					float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

					MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el nou voltatge al Working Electrode

					__NOP();

				}

			}

			measureCV=FALSE;

		} else {}

	}

	HAL_TIM_Base_Stop_IT(&htim2);             // Detenemos el timer al finalizar la medición

	HAL_ADC_Stop(&hadc1);					  //Paramos conversion adc

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

	__HAL_TIM_SET_COUNTER(&htim2, 0);         // Reiniciamos el contador del timer a 0

	estado = IDLE;                            // Reiniciamos variables
	point = 0;
	counter = 0;

}
