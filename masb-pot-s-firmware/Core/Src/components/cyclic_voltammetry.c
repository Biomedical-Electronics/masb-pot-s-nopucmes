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

	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);  

	HAL_TIM_Base_Start_IT(&htim2);           // Iniciamos el timer

	uint32_t cycles = 0; // we start at 0, when a cycle it has been done we will add 1 to this variable and get out of the loop

	measureCV = FALSE; // Con esta variable controlaremos cada vez que se active el callback del timer para hacer una medida

	ts = cvConfiguration.eStep/cvConfiguration.scanRate; // el sampling period viene definido como el cuociente ente estos dos inputs

	double vcell = cvConfiguration.eBegin; // igualamos el potencial de celda al introducido como input en eBegin

	double eStep = (cvConfiguration.eBegin < cvConfiguration.eVertex1) ? cvConfiguration.eStep : -cvConfiguration.eStep; 
	// con esta variable controlamos el signo de la suma de eStep partiendo del inicio, si el objetivo es mayor que el inicial sumamos, al contrario, restamos.
	double sign = (cvConfiguration.eBegin < cvConfiguration.eVertex1) ? 1 : -1;
	// con esta guardamos el signo, si el objetivo es mayor que el inicial positivo, a la inversa, negativo

	double vobj = cvConfiguration.eVertex1;       // Igualamos vObjetivo a eVertex1


	while(cycles < cvConfiguration.cycles){ // mientras el numero de ciclos sea menor que el introducido como input por el usuario

		if (measureCV==TRUE){ // si se ha activado el callback del timer, y por lo tanto, se ha llevado a cabo una medida, entraremos en este condicional

			vcell = (roundf(vcell * 100) / 100);  // Da problemas si no redondeamos vcell porque no entra en el bucle

			if (sign * (vcell - vobj) >= 0 ) { // Aquí simplemente comprobamos si se ha llegado ya al potencial objetivo o no
				// mientras vcell sea menor que vobj de subida la resta será negativa, multiplicada por un positivo porqué estaremos sumando
				// en caso contrario que estemos de bajada (vobj<vcell), hasta que vcell no supere vobj en valor absoluto, al estar multiplicado por un negativo, no entraremos en la condición

				vcell = vobj; // en caso que hayamos entrado, definimos vcell como vobj para tomar la medida en el punto objetivo exacto
				__NOP();

				if (vobj==cvConfiguration.eVertex1){ // si el objetivo es el vértice 1, definimos como nuevo objetivo el Vértice 2
					vobj=cvConfiguration.eVertex2;

					eStep = -eStep; // canviamos de signo la suma eStep: si sumábamos, pasamos a restar (eVertex2 < eVertex1), y si restábamos pasamos a sumar (eVertex2 > eVertex1)
					sign = -sign; // canviamos también el signo del pendiente

					__NOP();

				}

				else{ // si el objetivo no es el vértice 1...
					if (vobj==cvConfiguration.eVertex2){ // o es el vértice 2
						vobj=cvConfiguration.eBegin; // con lo que definiríamos el nuevo vobj como el eBegin
						
						eStep = -eStep; // y cambiariamos novamente de signo la suma de eStep i el valor del signo
						sign = -sign;

						__NOP();
					}

					else { // o es el eBegin, hecho que indicaria que se ha completado un ciclo
						vobj=cvConfiguration.eVertex1; // indicamos como nuevo vobj el potencial del vértice 1 (si se quiere más de 1 ciclo)
						cycles += 1;  // o le añadimos 1 a la variable cycles, haciendo que en la próxima iteracion no entre al contador 
						// (num of cycles == cvConfiguration.cycles) y salga del loop

					__NOP();

					}
				}

				vcell = vcell + eStep; // cada vez que llegamos a un objetivo, aplicamos ya el incremento/decremento para que la siguiente medida no se repita en el mismo punto

				__NOP();

			}

			else{ // si el potencial vcell no ha llegado al objetivo

				vcell = vcell + eStep; // aplicamos un incremento/decremento al potencial vcell (recordar que arriba definimos eStep como positivo o negativo segun el contexto)

				float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

				MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el nou voltatge al Working Electrode

				__NOP();

			}

		measureCV=FALSE; // una vez echo el incremento de vcell+eStep igualamos la variable a False para evitar que entre en el bucle hasta que no se active el callback del timer novamente

		HAL_ADC_Start(&hadc1); // iniciamos la conversion

		HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

		uint32_t measurement1 = HAL_ADC_GetValue(&hadc1);  //obtenemos primer valor adc

		double vcell = (1.65- ((double)measurement1)*3.3/(1023.0))*2.0;          // formula 2 MIRARLO

		HAL_ADC_Start(&hadc1); // iniciamos la conversion

		HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos que finalice la conversion

		uint32_t measurement2 = HAL_ADC_GetValue(&hadc1);  //obtenemos segundo valor adc

		double icell=(((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // formula 3 (dividido rtia)

		counter = counter + ts*1000;

		data.point=point_CV;
		data.timeMs=counter;
		data.voltage=vcell;
		data.current=icell;

		MASB_COMM_S_sendData(data);

		point_CV++;

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
