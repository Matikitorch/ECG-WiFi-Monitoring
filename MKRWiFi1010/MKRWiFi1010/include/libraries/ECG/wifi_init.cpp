/*
 * wifi_init.cpp
 *
 * Created: 2/19/2021 5:03:45 PM
 *  Author: Justin Bee
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
	
	for (;;)
	{
		if ( ( WiFi.RSSI() == 0 ) && ( isConnected == true ) )
		{
			isConnected = false;
			led_mode(led_mode_faulted);
			vTaskDelay(50);
			//TODO suspend the wifi communication task
		}
		else
		{
			// Do nothing
		}
		if ( !isConnected )
		{
			led_mode(led_mode_connecting);
			vTaskDelay(1000); 
			Serial.print("\r\nAttempting to connect to SSID: ");
			Serial.print(ssid);
			Serial.print(" Password ");
			Serial.println(pass);
			status = WiFi.begin(ssid, pass);		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
			if( status != WL_CONNECTED )
			{
				Serial.println("Not connected");
			}
			else
			{
				Serial.println("Connected to wifi");
				isConnected = true;
				led_mode(led_mode_normal);  
				vTaskDelay(50);
				//TODO - start the Wifi Communication Task
			}		
		}
		vTaskDelay(5000); //wait 5 seconds
	}
	vTaskDelete( NULL );
}