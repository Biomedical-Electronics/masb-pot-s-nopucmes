/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */
#define MCP4725_ID 0x60
#include "components/chronoamperometry.h"
void chronoamp(struct CA_Configuration_S caConfiguration){
	double eDC=caConfiguration.eDC; //leemos edc del parámetro de entrada
	Voltage2Dac(eDC); //
}
