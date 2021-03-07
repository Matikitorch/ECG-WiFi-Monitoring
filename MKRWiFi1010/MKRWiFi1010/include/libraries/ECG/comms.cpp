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


const char server[] = "tqmlv0dfbh.execute-api.us-east-1.amazonaws.com/dev/";
//const char server[] = "master.d30zp0554c2v1e.amplifyapp.com"; //this one works after adding the certificate through Arduino IDE


//**************************************************
//
//  Local Declarations
//
//****************************************************
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
	//WiFiSSLClient client;
	WiFiClient client;
	
	//String contentType = "application/x-www-form-urlencoded";
	//String postData = "serial=testSerial&adc=12.1&temp=98.6";
	/*
	POST https://tqmlv0dfbh.execute-api.us-east-1.amazonaws.com/dev/

	Content-Type : application/json

	{
		"serial":"Test",
		"adc":"45",
		"temp":"97.8"
	}
	*/
	String postEntity = String("serial=test&adc=1234&temp=9999");
	for(;;)
	{
		if(IsWifiConnected())
		{
			//Close the connection in case it is still open
			client.stop();
			Serial.println("Attempting to connect to server....");
			if (client.connect(server, 443)) {
				Serial.println("connected to server");
				//client.println("GET / HTTP/1.1");
				//client.print("Host: ");
				//client.println(server);
				//client.println("User-Agent: ArduinoWiFi/1.1");
				//client.println("Connection: close");
				//client.println();
				
				// Make a HTTP request:
				/*client.println("POST / HTTP/1.1");
				//serial=testSerial&adc=12.1&temp=98.6
				//client.println("Host: www.google.com");
				//client.println("Connection: close");
				//client.println("POST  HTTP/1.1");
				client.print("Host: ");
				client.println(server);
				client.println("User-Agent: ArduinoWiFi/1.1");
				client.println("Connection: close");
				client.println("Content-Type: application/x-www-form-urlencoded;");
				client.print("Content-Length: ");
				client.println(postEntity.length());
				client.println();
				client.println(postEntity); */
			}
			else{
				Serial.println("Not connected to server");
			}
			//client.post("/post", contentType, postData);
		}
		vTaskDelay(5000);
	}
	
	
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