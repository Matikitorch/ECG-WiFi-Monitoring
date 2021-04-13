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
char ssid[] = "NETGEAR42";  //enter your SSID here
char pass[] = "bravecheese823";  //enter your password here
int status = WL_IDLE_STATUS;
bool isConnected = false;

//---------------------------------------------------------------
//
// void task_WifiInitialize(void *pvParameters)
//
// Author: Justin Bee
// This task connects to the wifi network if not already connected
//---------------------------------------------------------------
void task_WiFiInitialize(void *pvParamters)
{
	led_mode(led_mode_starting);
	vTaskDelay(1000); //1 second wait for startup
	for (;;)
	{
		if ( ( WiFi.status() ==  WL_CONNECTION_LOST ) )
		{
			isConnected = false;
			led_mode(led_mode_faulted);
			vTaskDelay(500);
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
			Serial.println("XXXXX");
			vTaskDelay(1000); //1 second wait to allow LED behavior to run correctly
			status = WiFi.begin(ssid, pass);		// Connect to WPA/WPA2 network. 
			if( status != WL_CONNECTED )
			{
				Serial.println("Not connected");
				
				led_mode(led_mode_connecting);
				//sleep for 500 ms
				vTaskDelay(500);
			}
			else
			{
				Serial.println("Connected to wifi");
				isConnected = true;
				//Set the LED mode to normal solid green
				led_mode(led_mode_normal);
				//Create the WiFi communication task
				xTaskCreate(task_WiFiComm,    "WiFi Comm",        256,    NULL,   TASK_PRIORITY_NORMAL,   NULL);
				//sleep for 500 ms
				vTaskDelay(500);
			}
		}
		else
		{
			vTaskDelay(500); //wait 500 ms
		}	
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
