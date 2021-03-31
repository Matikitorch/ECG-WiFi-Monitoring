/*
 * wifi_init.cpp
 *
 * Created: 2/28/2021 7:17:57 PM
 *  Author: jbeea
 */ 



#include "FreeRTOS_SAMD21.h"
#include "hdr.h"
#include "wifi_init.h"
#include "WiFi.h"
#include "led.h"
#include "comms.h"
#include "adc.h"

//---------------------------------------------------------------
//  Local variables
//---------------------------------------------------------------
char ssid[] = "b33zhive";  //enter your SSID here
char pass[] = "Lyla_Bee_3";  //enter your password here
int status = WL_IDLE_STATUS;
bool isConnected = false;

//---------------------------------------------------------------
//
// void task_WifiInitialize(void *pvParameters)
//
// This task connects to the wifi network if not already connected
//---------------------------------------------------------------
void task_WiFiInitialize(void *pvParamters)
{
	led_mode(led_mode_starting);
	//vTaskDelay(10000); //10 second wait for startup
	
	for (;;)
	{
		if ( ( WiFi.status() ==  WL_CONNECTION_LOST ) )
		{
			isConnected = false;
			led_mode(led_mode_faulted);
			vTaskDelay(500);
			//TODO suspend the wifi communication task
		}
		else
		{
			// Do nothing
		} 
		if ( !isConnected )
		{
			
			led_mode(led_mode_connecting);
			Serial.print("\r\nAttempting to connect to SSID: ");
			Serial.print(ssid);
			Serial.print(" Password ");
			Serial.println(pass);
			//vTaskDelay(10000); //10 second wait to allow LED behavior to run correctly
			status = WiFi.begin(ssid, pass);		// Connect to WPA/WPA2 network. 
			if( status != WL_CONNECTED )
			{
				Serial.println("Not connected");
				led_mode(led_mode_connecting);
				vTaskDelay(500);
			}
			else
			{
				Serial.println("Connected to wifi");
				isConnected = true;
				led_mode(led_mode_normal);
				//TODO - start the Wifi Communication Task
				xTaskCreate(task_WiFiComm,    "WiFi Comm",        512,    NULL,   TASK_PRIORITY_NORMAL,   NULL);
				vTaskDelay(1000);
			}
		}
		else
		{
			//Do nothing
		}
		vTaskDelay(1000); //wait 1 seconds
	}
	vTaskDelete( NULL );
}

//********************************************************
//
// bool IsWifiConnected()
//
// Author: Justin Bee
// Date: 3/06/2021
// Returns true is connected, false otherwise
//********************************************************
bool IsWifiConnected()
{
	return isConnected;
}
