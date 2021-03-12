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



//**************************************************
//
//  Local Declarations
//
//****************************************************
const char server[] = "tqmlv0dfbh.execute-api.us-east-1.amazonaws.com";

String JSONBuilder(void);




//*******************************************************
//
// void task_WiFiComm(void *pvParameters)
// 
// Author: Justin Bee
// Date:3/06/2021
// This task handles the WiFi communication. It calls JSONBuilder() for the string
// Sends the data to the webapp
//*******************************************************************
void task_WiFiComm(void *pvParamters)
{
	WiFiClient client;
	
	String postEntity = String("{\"serial\":\"Default-001\",\"adc\":\"45\",\"temp\":\"97.8\"}");
	for(;;)
	{
		if(IsWifiConnected())
		{
			//Close the connection in case it is still open
			client.stop();
			Serial.println("Attempting to connect to server....");
			if( client.connectSSL(server,443) ){
				Serial.println("connected to server");
				// Make a HTTP POST request:
				client.println("POST /dev HTTP/1.1");
				// Host
				client.print("Host: ");
				client.println(server);
				client.println("Connection: close");
				client.println("Content-Type: application/json");
				client.println("Content-Length: 49");
				//client.print("Content-Length: ");
			    //client.println(postEntity.length());
				client.println();
				client.println(postEntity);
				client.println(); 
				client.println();
				
				 while(client.available()==0)//wait until data reception
				 {
					 
				 }
				 while(client.available()) //write all incoming data characters on serial monitor
				 {
					 char codeResponse = client.read();
					 Serial.write(codeResponse);
				 } 
				// client.stop();
			}
			else{
				Serial.println("Not connected to server");
			}
		}
		vTaskDelay(5000);
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
	
}