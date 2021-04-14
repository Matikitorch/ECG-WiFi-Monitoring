//=============================================================================
//
// 
//=============================================================================

#include "Sketch.h"
#include <FreeRTOS_SAMD21.h>

#include "hdr.h"
#include "init.h"
#include "led.h"
#include "utilities.h"



//=============================================================================
//                                 SETUP
//
//=============================================================================

void setup(void) {

	pinMode(LED_BUILTIN, OUTPUT);	// initialize digital pin LED_BUILTIN as an output.
	//set the led mode to starting
	led_mode(led_mode_starting);
	
// Open serial communications and wait for port to open:
	Serial.begin(115200);
	while (!Serial) {				// wait for serial port to connect. Needed for native USB port only
		digitalWrite(LED_BUILTIN, HIGH);
		delay(50);
		digitalWrite(LED_BUILTIN, LOW);
		delay(50);
	}
	delay(500);
	
	Serial.println(F("\r\n=============================================="));
	Serial.println(F("    ECG Wifi Monitor"));
	Serial.println(F("    Authors: Matias Blanc and Justin Bee"));
	Serial.println(F("    Copyright: 2021"));
	Serial.println(F("=============================================="));
	Serial.print(F("Serial Number: "));
	Serial.println(serialNumber);

 	SPI.begin();					// initialize the SPI:
	Wire.begin();					// intialize the I2C
	Wire.setClock(3400000);         //set the I2C speed
	BCsetup();

	Serial.println(F("==============================================\r\n"));
	
	// Creates initialization task
	xTaskCreate(task_Initialize,	"Initialize",	512,	NULL,	TASK_PRIORITY_HIGH, NULL);

	// Start the RTOS, this function will never return and will schedule the tasks.
	vTaskStartScheduler();
	
	//digitalWrite(LED_BUILTIN, HIGH);

}









