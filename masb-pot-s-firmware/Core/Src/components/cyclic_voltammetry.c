/**
  ******************************************************************************
  * @file		cyclic_voltammetry.c
  * @brief		Gestión de la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/cyclic_voltammetry.h"

#include "components/stm32main.h"          // Para utilizar el setup()

#include "math.h"                          //Para utilizar roundf‍

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

	/*

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	uint32_t measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

	double vcell = (1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

	HAL_ADC_Start(&hadc1); // iniciamos la conversion

	HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

	uint32_t measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

	double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)

	data.point=7;
	data.timeMs=7;
	data.voltage=7.0;
	data.current=7.0;

	MASB_COMM_S_sendData(data);

*/

	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

	HAL_TIM_Base_Start_IT(&htim2);           // Iniciamos el timer

	uint32_t cycles = 0; // we start at 0, when a cycle it has been done we will add 1 to this variable and get out of the loop

	measureCV = FALSE;

	ts = cvConfiguration.eStep/cvConfiguration.scanRate;

	// Double lo hemos cambiado por int32_t

	double vcell = cvConfiguration.eBegin;


	double eStep = (cvConfiguration.eBegin < cvConfiguration.eVertex1) ? cvConfiguration.eStep : -cvConfiguration.eStep;
	double sign = (cvConfiguration.eBegin < cvConfiguration.eVertex1) ? 1 : -1;

	double vobj = cvConfiguration.eVertex1;       // Igualamos vObjetivo a eVertex1


	while(cycles < cvConfiguration.cycles){

		if (measureCV==TRUE){

			vcell = (roundf(vcell * 100) / 100);  // Da problemas si no redondeamos vcell porque no entra en el bucle

			if (sign * (vcell - vobj) >= 0 ) {

				vcell = vobj;
				__NOP();

				if (vobj==cvConfiguration.eVertex1){
					vobj=cvConfiguration.eVertex2;

					eStep = -eStep;
					sign = -sign;

					__NOP();

				}

				else{
					if (vobj==cvConfiguration.eVertex2){
						vobj=cvConfiguration.eBegin;

						eStep = -eStep;
						sign = -sign;

						__NOP();
					}

					else {
						vobj=cvConfiguration.eVertex1;
						cycles += 1;   // when vobj equals eBegin, means a cycle has been done, if we add one to cycles count,
									   // and the total num of cycles == cvConfiguration.cycles, we will get out of the loop
					__NOP();

					}
				}

				vcell = vcell + eStep;

				__NOP();

			}

			else{

				vcell = vcell + eStep;

				float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

				MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el nou voltatge al Working Electrode

				__NOP();

			}
		measureCV=FALSE;

		}

	}


/* ¡¡QUITAR!!
	while(cycles < cvConfiguration.cycles){

		if (measureCV==TRUE){

			__NOP();

			if ((roundf(vcell * 100) / 100)==vobj){

				__NOP();

				if (vobj==cvConfiguration.eVertex1){
					vobj=cvConfiguration.eVertex2;

					__NOP();

				} else{
					if (vobj==cvConfiguration.eVertex2){
						vobj=cvConfiguration.eBegin;

						__NOP();

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

		} else {

		}

	}*/

	HAL_TIM_Base_Stop_IT(&htim2);             // Detenemos el timer al finalizar la medición

	HAL_ADC_Stop(&hadc1);					  //Paramos conversion adc

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

	__HAL_TIM_SET_COUNTER(&htim2, 0);         // Reiniciamos el contador del timer a 0

	estado = IDLE;                            // Reiniciamos variables
	point_CV = 1;
	counter = 0;

}
