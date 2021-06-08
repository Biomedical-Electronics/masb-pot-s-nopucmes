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

uint32_t point_CV = 1;    // variable punto para la voltametria ciclica

void Voltammetry_Config(struct CV_Configuration_S cvConfiguration){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);   // Cerramos rele (EN - HIGH (1))

	float vdac = (float)(1.65-(cvConfiguration.eBegin/2.0));   // Calculamos el valor de vdac en función de eBegin

	MCP4725_SetOutputVoltage(hdac, vdac);      // Fijamos el valor de Vcell a eBegin

	estado = CV;    // Definimos la variable estado como CV (utilizamos esta variable en el timer)

	__HAL_TIM_SET_AUTORELOAD(&htim2, (cvConfiguration.eStep/cvConfiguration.scanRate*1000)*84000); // sampling period defined by eStep/scanRate

}

void Voltammetry_Value(struct CV_Configuration_S cvConfiguration){

	// Configuracion del primer punto a tiempo 0 previo a activar el timer:
	// Realizamos las mediciones de Vcell_real y Icell

	HAL_ADC_Start(&hadc1);                     // iniciamos la conversion
	HAL_ADC_PollForConversion(&hadc1, 200);    // esperamos que finalice la conversion

	uint32_t measurement1 = HAL_ADC_GetValue(&hadc1);              // Obtenemos primer valor adc

	double vcell_real = (1.65- ((double)measurement1)*3.3/(1023.0))*2.0;  // Obtenemos el valor de Vcell

	HAL_ADC_Start(&hadc1);                     // iniciamos la conversion
	HAL_ADC_PollForConversion(&hadc1, 200);    // esperamos que finalice la conversion

	uint32_t measurement2 = HAL_ADC_GetValue(&hadc1);                         //obtenemos segundo valor adc

	double icell = (((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // Obtenemos el valor de Icell (todo dividido entre rtia)

	/*
	 * Enviamos el primer punto (0) que corresponderá a tiempo 0 y que tendrá
	 * los valores de Vcell y Icell calculados previamente
	 */

	data.point=point_CV;
	data.timeMs=0;
	data.voltage=vcell_real;
	data.current=icell;

	MASB_COMM_S_sendData(data);        // Enviamos los datos

	point_CV++;                        // Vamos al punto 2 de CV


	// Despues de enviar este punto activamos el timer y empezamos el resto de mediciones:

	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);  

	HAL_TIM_Base_Start_IT(&htim2);    // Iniciamos el timer

	uint32_t cycles = 0; // Iniciamos con el contador de ciclos a 0, cada vez que realicemos un ciclo le sumaremos 1 a esta variable

	measureCV = FALSE;   // Con esta variable controlaremos cada vez que se active el callback del timer para hacer una medida

	ts = cvConfiguration.eStep/cvConfiguration.scanRate; // el sampling period viene definido como el cuociente ente estos dos inputs

	double vcell = cvConfiguration.eBegin; // igualamos el potencial de celda al introducido como input en eBegin

	// Con esta variable controlamos el signo de la suma de eStep partiendo del inicio, si el objetivo es mayor que el inicial sumamos, al contrario, restamos.
	double eStep = (cvConfiguration.eBegin < cvConfiguration.eVertex1) ? cvConfiguration.eStep : -cvConfiguration.eStep; 

	// Con esta guardamos el signo, si el objetivo es mayor que el inicial positivo, a la inversa, negativo
	double sign = (cvConfiguration.eBegin < cvConfiguration.eVertex1) ? 1 : -1;

	double vobj = cvConfiguration.eVertex1;       // Igualamos vObjetivo a eVertex1


	while(cycles < cvConfiguration.cycles){   // mientras el numero de ciclos sea menor que el introducido como input por el usuario

		if (measureCV==TRUE){    // si se ha activado el callback del timer, y por lo tanto, se ha llevado a cabo una medida, entraremos en este condicional

			vcell = (roundf(vcell * 100) / 100);       // Da problemas si no redondeamos vcell porque no entra en el bucle

			/*
			 * mientras vcell sea menor que vobj de subida la resta será negativa, multiplicada por un positivo porqué estaremos sumando
			 * en caso contrario que estemos de bajada (vobj<vcell), hasta que vcell no supere vobj en valor absoluto, al estar
			 * multiplicado por un negativo, no entraremos en la condición
			 */

			if (sign * (vcell - vobj) >= 0 ) {

				vcell = vobj; // en caso que hayamos entrado, definimos vcell como vobj para tomar la medida en el punto objetivo exacto
				__NOP();

				if (vobj==cvConfiguration.eVertex1){   // si el objetivo es el vértice 1, definimos como nuevo objetivo el Vértice 2
					vobj=cvConfiguration.eVertex2;

					eStep = -eStep;  // cambiamos de signo la suma eStep: si sumábamos, pasamos a restar (eVertex2 < eVertex1), y si restábamos pasamos a sumar (eVertex2 > eVertex1)
					sign = -sign;    // cambiamos también el signo de la pendiente

					__NOP();

				}

				else{   // si el objetivo no es el vértice 1...

					if (vobj==cvConfiguration.eVertex2){   // o es el vértice 2
						vobj=cvConfiguration.eBegin;       // con lo que definiríamos el nuevo vobj como el eBegin
						
						eStep = -eStep;                    // y cambiaremos nuevamente de signo la suma de eStep y el valor del signo
						sign = -sign;

						__NOP();
					}

					else {                              // o es el eBegin, hecho que indicaria que se ha completado un ciclo
						vobj=cvConfiguration.eVertex1;  // indicamos como nuevo vobj el potencial del vértice 1
						cycles += 1;                    // o le añadimos 1 a la variable cycles

						if (cycles == cvConfiguration.cycles){
							break;
						}

						/*
						 * De esta manera, en caso de que no haya llegado al número de ciclos totales sumaremos 1 y
						 * iremos al vértice 1. En cambio, si al sumar uno al número de ciclos, saldremos del bucle
						 * while y no haremos más mediciones.
						 */

						// (num of cycles == cvConfiguration.cycles) y salga del loop

					__NOP();

					}
				}

				vcell = vcell + eStep; // cada vez que llegamos a un objetivo, aplicamos ya el incremento/decremento para que la siguiente medida no se repita en el mismo punto

				__NOP();

			}

			else {      // si el potencial vcell no ha llegado al objetivo

				if (sign * ((vcell+eStep) - vobj) >= 0 ) {

					vcell = vobj;

					__NOP();

				} else{

				vcell = vcell + eStep;                  // aplicamos un incremento/decremento al potencial vcell (arriba definimos eStep como positivo o negativo segun el contexto)

				float vdac = (float)(1.65-(vcell/2.0)); // definim el vdac a partir del Vcell que volem donar

				MCP4725_SetOutputVoltage(hdac, vdac);   // administrem el nou voltatge al Working Electrode

				__NOP();
				}

			}

		/*
		 * Una vez hecho el incremento de vcell+eStep igualamos la variable measureCV a FALSE para evitar
		 * que entre en el bucle de nuevo. Esta variable solo volverá a ser TRUE (y por tanto entraremos
		 * en el bucle) si se activa la función callback del timer.
		 */

		measureCV=FALSE;

		// Realizamos las mediciones de Vcell_real y Icell

		HAL_ADC_Start(&hadc1);                    // iniciamos la conversion ADC

		HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos 200 ms que finalice la conversion

		uint32_t measurement1 = HAL_ADC_GetValue(&hadc1);  // obtenemos el primer valor ADC

		vcell_real = (1.65- ((double)measurement1)*3.3/(1023.0))*2.0;    // transformamos el valor en ADC a Vcell

		HAL_ADC_Start(&hadc1);                    // iniciamos de nuevo la conversion ADC

		HAL_ADC_PollForConversion(&hadc1, 200);   // esperamos 200 ms que finalice la conversion

		uint32_t measurement2 = HAL_ADC_GetValue(&hadc1);  // obtenemos el primer valor ADC

		icell = (((((double)measurement2)*3.3/(1023.0))-1.65)*2.0)/10000.0;  // transformamos el valor en ADC a Icell

		counter = counter + ts*1000;              // variable counter que nos llevará la cuenta de los Ms

		data.point=point_CV;   // Punto de la medición que estamos enviando
		data.timeMs=counter;   // Ms que llevamos desde el inicio de la conversión
		data.voltage=vcell_real;    // Vcell obtenido
		data.current=icell;    // Icell obtenido

		MASB_COMM_S_sendData(data); // Enviamos los datos

		point_CV++;            // Sumamos uno al contador de puntos

		}

	}

	HAL_TIM_Base_Stop_IT(&htim2);             // Detenemos el timer al finalizar la medición

	HAL_ADC_Stop(&hadc1);					  // Paramos la conversion ADC

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);  // Abrimos rele (EN - LOW (0))

	__HAL_TIM_SET_COUNTER(&htim2, 0);         // Reiniciamos el contador del timer a 0

	// Reiniciamos variables a los valores iniciales
	estado = IDLE;
	point_CV = 1;
	counter = 0;

}
