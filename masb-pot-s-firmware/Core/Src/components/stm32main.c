/*
 * stm32main.c
 *
 *  Created on: 7 may. 2021
 *      Author: Usuario
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

struct CV_Configuration_S cvConfiguration;
struct CA_Configuration_S caConfiguration;
struct Data_S data;

AD5280_Handle_T hpot = NULL;
MCP4725_Handle_T hdac = NULL;

uint16_t estado = IDLE;

void setup(struct Handles_S *handles) {       // Esta parte se ejecutara una vez

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);  // Habilitamos la PMU (1 = PMU habilitada)

	MASB_COMM_S_setUart(handles->huart);

	MASB_COMM_S_waitForMessage();

	//================================== I2C =======================================
	I2C_Init(&hi2c1);

	//=========================== Potenciometro ====================================
	// Creamos el handle de la libreria.
	hpot = AD5280_Init();

	// Configuramos su direccion I2C de esclavo, su resistencia total (hay
	// diferentes modelos; este tiene 50kohms) e indicamos que funcion queremos que
	// se encargue de la escritura a traves del I2C. Utilizaremos la funcion
	// I2C_Write de la libreria i2c_lib.
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_Write);

	// Fijamos la resistencia de, por ejemplo, 12kohms.
	AD5280_SetWBResistance(hpot, 12e3f);

	//================================== DAC =======================================
	// Creamos el handle de la libreria.
	hdac = MCP4725_Init();

	// Configuramos la direccion I2C de esclavo, su tension de referencia (que es la
	// de alimentacion) e indicamos que funcion queremos que se encargue de la
	// escritura a traves del I2C. Utilizaremos la funcion I2C_Write de la libreria
	// i2c_lib.
	MCP4725_ConfigSlaveAddress(hdac, 0x66);
	MCP4725_ConfigVoltageReference(hdac, 4.0f);
	MCP4725_ConfigWriteFunction(hdac, I2C_Write);

	/*
	 * Variable estado nos definirá el modo (CA o CV) dentro del ISR del timer
	 */

}


void loop(void) {		// Esta parte se ejecutara de manera continuada

	if (MASB_COMM_S_dataReceived()) {   // Si se ha recibido un mensaje...

		switch(MASB_COMM_S_command()) { // Miramos que comando hemos recibido

			case START_CV_MEAS:         // Si hemos recibido START_CV_MEAS

				// Leemos la configuracion que se nos ha enviado en el mensaje y
				// la guardamos en la variable cvConfiguration
				cvConfiguration = MASB_COMM_S_getCvConfiguration();

				// Configuramos la Voltametria Ciclica
				Voltammetry_Config(cvConfiguration);

				__NOP();

				// Obtenemos los valores de la Voltametria Ciclica
				Voltammetry_Value(cvConfiguration);

				__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint

	 		// Aqui iria todo el codigo de gestion de la medicion que hareis en el proyecto
	        // si no quereis implementar el comando de stop.

				break;

			case START_CA_MEAS:         // Si hemos recibido START_CA_MEAS

				// Leemos la configuracion que se nos ha enviado en el mensaje y
				// la guardamos en la variable caConfiguration
				caConfiguration = MASB_COMM_S_getCaConfiguration();

				// Configuramos la Cronoamperometria
				Chronoamperometry_Config(caConfiguration);

				__NOP();

				// Obtenemos los valores de la Cronoamperometria
				Chronoamperometry_Value(caConfiguration);

				__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint

				break;

			case STOP_MEAS:            // Si hemos recibido STOP_MEAS

				__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint

	 		// Aqui iria el codigo para tener la medicion si implementais el comando stop.

				break;

	 		default: // En el caso que se envia un comando que no exista

				break;

		}

	// Una vez procesado los comando, esperamos el siguiente mensaje
	MASB_COMM_S_waitForMessage();

	}

	// Aqui es donde deberia de ir el codigo de control de las mediciones si se quiere implementar
	// el comando de STOP.

}
