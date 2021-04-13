/*
 * Init.c
 *
 * Created: 2/16/2021 4:50:35 PM
 *  Author: m.blanc
 */ 

#include <stdint.h>
#include <string.h>

#include "FreeRTOS_SAMD21.h"
#include "hdr.h"
#include "comms.h"
#include "led.h"
#include "adc.h"
#include "temp.h"
#include "wifi_init.h"

//*****************************************************************
// Call to initialize
//*****************************************************************
void task_Initialize( void *pvParameters ) 
{
	// Start all tasks
	xTaskCreate(task_LEDInitialize,		"LED Init",			256,	NULL,	TASK_PRIORITY_NORMAL,	NULL);
	xTaskCreate(task_WiFiInitialize,    "WiFi Init",        256,    NULL,   TASK_PRIORITY_NORMAL,   NULL);
	//xTaskCreate(task_ADCInitialize,		"ADC Init",			256,	NULL,	TASK_PRIORITY_NORMAL,	NULL);
	xTaskCreate(task_TempInitialize,	"Temp Init",		256,	NULL,	TASK_PRIORITY_NORMAL,	NULL);
		
	vTaskDelete( NULL );
}