/*
 * temp.cpp
 */ 

#include <stdlib.h>

#include <WiFiNINA.h>

#include "FreeRTOS_SAMD21.h"

#include "hdr.h"
#include "temp.h"

float tempC;
float tempF;

/*
 * Collects the temperature data
 *
 * TMP36 line of best fit V = 0.0093T + 0.5333
 * 
 * Arduino MKR WiFi 1010 input pin goes from 0V
 * to 3.3V and analogRead() will go from 0 to 1023.
 */
void task_TempRun(void *pvParameters)
{
	
	for(;;)
	{
		noInterrupts();
		uint32_t tempADC = analogRead(A2);
		interrupts();
		tempC = ((0.0032258 * (float)tempADC) - 0.5333) / 0.0093;
		tempF = (1.8 * (float)tempC) + 32;
		
		vTaskDelay(500);
	}
}

/*
 * Initializes the Temp
 */
void task_TempInitialize(void *pvParameters)
{	
	xTaskCreate(task_TempRun,		"Temp",		256,		NULL,		TASK_PRIORITY_HIGH,		NULL);
	
	vTaskDelete(NULL);
}

//********************************************************
//
// float getTempp()
//
// Author: Justin Bee
// Date: 3/27/2021
// Returns the value of tempF
//********************************************************
float getTemp()
{
	return tempF;
}