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
		noInterrupts();
		if(IsWifiConnected())
		{
			
			while( isConnectedServer == false )
			{
				Serial.println("Attempting to connect to server....");
				if (cl.connectSSL(server,443))
				{
					Serial.println("connected to server");
					xTaskCreate(task_WiFiSend,    "WiFi Send",        256,    NULL,   TASK_PRIORITY_NORMAL,   NULL);
					xTaskCreate(task_ADCInitialize,		"ADC Init",			256,	NULL,	TASK_PRIORITY_NORMAL,	NULL);
		
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
	
	
	for(;;)
	{
		noInterrupts();
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
		//cl.println();
		//Serial.println(postString);
		//postString = ""; //reset the string
		
			
		//while(cl.available()==0)//wait until data reception
		//{
					
		//}
		
		while(cl.available()) //write all incoming data characters on serial monitor
		{
			char codeResponse = cl.read();
			Serial.write(codeResponse);
		}
		//Serial.println(cl.connected());		
		/*if(cl.connected() == CLOSED)
		{
			isConnectedServer = false;
			forceRestartWiFi = true;
			WiFi.disconnect();
		}*/
		interrupts();
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
	String postEntity = String("{\"serial\":\"" +String(serialNumber)+"\",\"adc\":\""+String(getAin())+"\",\"temp\":\""+ String(getTemp())+ "\"}");
		
	return postEntity;
}



char * packetBuilder(void)
{
	String postString = JSONBuilder();
	String lengthPostString = String(postString.length());
	char postEntity[] = "POST /dev HTTP/1.1\nHost: tqmlv0dfbh.execute-api.us-east-1.amazonaws.com\nConnection: keep-alive\nContent-Type: application/json\nContent-Length: ";
	strcat(postEntity, lengthPostString.c_str());
	strcat(postEntity, "\n\n");
	strcat(postEntity, postString.c_str());
	strcat(postEntity, "\n");
	//Serial.println(postEntity);
	return postEntity;
	
}



/*void task_WiFiComm(void *pvParamters)
{
	xTaskCreate(task_ADCInitialize,		"ADC Init",			256,	NULL,	TASK_PRIORITY_NORMAL,	NULL);

	for (;;)
	{
		wf.begin(2430);
		//char temp[]  = packetBuilder();
		String postString = JSONBuilder();
		String lengthPostString = String(postString.length());
		char postEntity[] = "POST /dev HTTP/1.1\nHost: tqmlv0dfbh.execute-api.us-east-1.amazonaws.com\nConnection: keep-alive\nContent-Type: application/json\nContent-Length: 50 \n\n {\"serial\":\"Default-001\",\"adc\":\"0\",\"temp\":\"124.20\"}\n";
		//char postEntity[] = "POST /dev HTTP/1.1\nHost: tqmlv0dfbh.execute-api.us-east-1.amazonaws.com\nConnection: keep-alive\nContent-Type: application/json\nContent-Length: ";
		//strcat(postEntity, lengthPostString.c_str());
		//strcat(postEntity, "\n\n");
		//strcat(postEntity, postString.c_str());
		//strcat(postEntity, "\n");
		Serial.println("Attempting UDP packet transmission");
	    //Serial.println(postEntity);
		wf.beginPacket(server, 443);
		wf.write(postEntity, (sizeof(postEntity)-26u));
		wf.endPacket();
		Serial.println(wf.parsePacket());
		vTaskDelay(4); // 4 =  250 HZ
	}
}  */

