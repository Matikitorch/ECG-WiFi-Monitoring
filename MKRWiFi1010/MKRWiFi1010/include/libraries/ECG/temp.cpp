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
		//static int toggle;
		//digitalWrite(A5, toggle);
		//toggle ^= 0xFFFFFFFF;
		
		noInterrupts();
		uint32_t tempADC = analogRead(A2);
		interrupts();
		
		tempC = 3300.0 * (tempADC / 1024.0); // Convert the FS of the temperature sensor into 0-3300mV
		tempC = (tempC - 500.0) / 10.0; // Remove the offset of TMP36 and convert from mV to C
		
		tempF = (1.8 * (float)tempC) + 32;
		
		Serial.print("Temperature = ");
		
		Serial.print(tempC, 1);
		Serial.print("C ");
		
		Serial.print(tempF, 1);
		Serial.print("F ");
		
		Serial.print("(");
		Serial.print(tempADC);
		Serial.print(")");
		Serial.println();
		
		vTaskDelay(50);
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