/*
 * wifi_init.cpp
 *
 * Created: 2/28/2021 7:17:57 PM
 *  Author: jbeea
 */ 



#include "FreeRTOS_SAMD21.h"

#include "wifi_init.h"
#include "WiFi.h"
#include "led.h"

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
// This task connects to the wifi network if not already connected
//---------------------------------------------------------------
void task_WiFiInitialize(void *pvParamters)
{
	led_mode(led_mode_starting);
	vTaskDelay(10000); //10 second wait for startup
	
	for (;;)
	{
		if ( ( WiFi.RSSI() == 0 ) && ( isConnected == true ) )
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
			vTaskDelay(10000); //10 second wait to allow LED behavior to run correctly
			status = WiFi.begin(ssid, pass);		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
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
				vTaskDelay(1000);
				//TODO - start the Wifi Communication Task
			}
		}
		vTaskDelay(1000); //wait 1 seconds
	}
	vTaskDelete( NULL );
}
