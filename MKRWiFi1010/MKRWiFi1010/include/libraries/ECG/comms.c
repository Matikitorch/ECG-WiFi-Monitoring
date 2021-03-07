/*
 * comms.c
 *
 * Created: 2/18/2021 10:01:22 PM
 *  Author: m.blanc
 */ 

#include "comms.h"
#include "WiFiNINA.h"

const char server[] = "https://tqmlv0dfbh.execute-api.us-east-1.amazonaws.com/dev/"

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
void task_WiFiInitialize(void *pvParamters)
{
	WiFiClient wifi;
	HttpClient client = HttpClient(wifi, serverName, 80);
	String contentType = "application/x-www-form-urlencoded";
	String postData = "serial=testSerial&adc=12.1&temp=98.6";
	for(;;)
	{
		client.post("/post", contentType, postData);
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