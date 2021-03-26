/*
 * comms.cpp
 *
 * Created: 3/6/2021 2:28:20 PM
 *  Author: jbeea
 */ 
#include <WiFiNINA.h>
#include "comms.h"
#include "FreeRTOS_SAMD21.h"
#include "hdr.h"
#include "wifi_init.h"



//********************************************************************************
//
//  Local Declarations
//
//********************************************************************************
const char server[] = "tqmlv0dfbh.execute-api.us-east-1.amazonaws.com";
WiFiClient cl;  

String JSONBuilder(void);




//********************************************************************************
//
// void task_WiFiComm(void *pvParameters)
// 
// Author: Justin Bee
// Date:3/06/2021
// This task attempts connection to server if not connected. Once connected
// start the WifiSend() task
//********************************************************************************
void task_WiFiComm(void *pvParamters)
{
	for(;;)
	{
		if(IsWifiConnected())
		{
			//Close the connection in case it is still open
			//client.stop();
			Serial.println("Attempting to connect to server....");
			if( !cl.connected() )
			{
				if (cl.connectSSL(server,443))
				{
					Serial.println("connected to server");
					xTaskCreate(task_WiFiSend,    "WiFi Send",        256,    NULL,   TASK_PRIORITY_NORMAL,   NULL);
				}
				else
				{
					Serial.println("not connected to server");
				}
			}
			else
			{
				//Do nothing connected	
			}
		}
		else
		{
			//Do nothing
		}
		vTaskDelay(5000);
	}
	vTaskDelete( NULL );
}

//********************************************************************************
//
// void task_WiFiSend(void *pvParameters)
//
// Author: Justin Bee
// Date:3/12/2021
// This task handles the WiFi communication. It calls JSONBuilder() for the string
// Sends the data to the webapp
//********************************************************************************
void task_WiFiSend(void *pvParamters)
{
	
	
	for(;;)
	{
		
		String postString = JSONBuilder();
		// Make a HTTP POST request:
		cl.println("POST /dev HTTP/1.1");
		// Host
		cl.print("Host: ");
		cl.println(server);
		cl.println("Connection: keep-alive");  //keep the connection open
		cl.println("Content-Type: application/json");
		cl.print("Content-Length: ");
		cl.println(String(postString.length()));
		cl.println(); //necessary to separate header from body
		cl.println(postString);
		cl.println();
		cl.println();
		
			
		while(cl.available()==0)//wait until data reception
		{
					
		}
		while(cl.available()) //write all incoming data characters on serial monitor
		{
			char codeResponse = cl.read();
			Serial.write(codeResponse);
		}

		vTaskDelay(4); // 4 =  250 HZ
	}
	vTaskDelete( NULL );
}


//***************************************************
//
// String JSONBuilder()
//
// Author: Justin Bee
// Date: 3/06/2021
// This function grabs the available data and builds the JSON string
//******************************************************************
String JSONBuilder(void)
{
	String postEntity = String("{\"serial\":\"Default-001\",\"adc\":\"45\",\"temp\":\"97.8\"}");
		
	return postEntity;
}

