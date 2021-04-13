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
#include "utilities.h"
#include "temp.h"
#include "adc.h"
#include "WiFiUdp.h"



//********************************************************************************
//
//  Local Declarations
//
//********************************************************************************
const char server[] = "tqmlv0dfbh.execute-api.us-east-1.amazonaws.com";
bool isConnectedServer = false;
bool tasksStarted = false;
WiFiClient cl;  
WiFiUDP wf;

String JSONBuilder(void);
char * packetBuilder(void);




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
		//Do not allow interupts while attempting SSL connection
		noInterrupts();
		if(IsWifiConnected())
		{
			
			while( isConnectedServer == false )
			{
				Serial.println("Attempting to connect to server....");
				if (cl.connectSSL(server,443))
				{
					Serial.println("connected to server");
					isConnectedServer = true;
					//check to see if the tasks were already created before attempting to create new tasks
					if(tasksStarted == false)
					{	
						xTaskCreate(task_WiFiSend,    "WiFi Send",        256,    NULL,   TASK_PRIORITY_NORMAL,   NULL);
						xTaskCreate(task_ADCInitialize,		"ADC Init",			256,	NULL,	TASK_PRIORITY_NORMAL,	NULL);
						tasksStarted = true;
					}
					else
					{
						//Do nothing
					}
				}
				else
				{
					Serial.println("not connected to server");
				}
			}			
		}
		else
		{
			//Do nothing
		}
		//Re-enable the interrupts before delaying the task
		interrupts();
		vTaskDelay(500);
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
	
	uint32_t count = 0;
	for(;;)
	{
		noInterrupts();
		if( cl.connected())
		{
			//String postString = JSONBuilder();
			String postString = "{\"serial\":\"Default-001\",\"adc\":\""+String(getAin())+"\",\"temp\":\""+ String(getTemp())+ "\"}";
			// Make a HTTP POST request:
			cl.println("POST /dev HTTP/1.1");
			// Host
			cl.println("Host: tqmlv0dfbh.execute-api.us-east-1.amazonaws.com");
			//cl.println(server);
			cl.println("Connection: keep-alive");  //keep the connection open
			cl.println("Content-Type: application/json");
			cl.print("Content-Length: ");
			cl.println(String(postString.length()));
			cl.println(); //necessary to separate header from body
			cl.println(postString);
			cl.println();
			
			while(cl.available()) //write all incoming data characters on serial monitor
			{
				char codeResponse = cl.read();
				Serial.write(codeResponse);
			}
			//Serial.println(count++);
		}
		else
		{
			isConnectedServer = false;
		}
		interrupts();
		vTaskDelay(2); // 4 =  250 HZ
	}
	vTaskDelete( NULL );
}


//*****************************************************************
//
// String JSONBuilder()
//
// Author: Justin Bee
// Date: 3/06/2021
// This function grabs the available data and builds the JSON string
//******************************************************************
String JSONBuilder(void)
{
	String postEntity = "{\"serial\":\"" +String(serialNumber)+"\",\"adc\":\""+String(getAin())+"\",\"temp\":\""+ String(getTemp())+ "\"}";
	return postEntity;
}



