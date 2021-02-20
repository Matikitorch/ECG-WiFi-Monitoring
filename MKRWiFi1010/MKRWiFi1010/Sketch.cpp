//=============================================================================
// This project is the basis for projects with the MKRWiFi1010 board
//
// Tested on MKR WiFi1010
//-----------------------------------------------------------------------------
// LIBRARY:
//
// Contains library from Arduino IDE (1.8.13) :
// C:\Users\<username>\AppData\Local\Arduino15\packages\arduino\hardware\samd\1.8.11
// C:\Program Files (x86)\Arduino
// 
// MCP23017 Library:
// url=https://github.com/blemasle/arduino-mcp23017 
//
// Ethernet Library:
// url=https://github.com/OPEnSLab-OSU/EthernetLarge
// 
// SSL Client
// url= https://github.com/OPEnSLab-OSU/SSLClient
//
// Removed all "namespace arduino" and "arduino::" from all library
//-----------------------------------------------------------------------------
// NOTE:
// Vectors are in: cortex_handlers.c  and samd21g18a.h
// Use samd21g18a.h
//
//=============================================================================

#include "Sketch.h"
#include <FreeRTOS_SAMD21.h>

#include "hdr.h"
#include "init.h"
#include "led.h"



//=============================================================================
//                                 SETUP
//
//=============================================================================

void setup() {

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

//SPI
 	Serial.println(F("SPI setup"));
 	SPI.begin();					// initialize SPI:

// I2C
	Serial.println(F("WIRE setup"));
	Wire.begin();					// join i2c bus (address optional for master)
	
	i2c_scan();		// Show list of I2C peripheral

// Battery chargher chip
	BCsetup();

	
	Serial.println(F("================== END SETUP =================\r\n"));
	
	// Creates initialization task
	xTaskCreate(task_Initialize,	"Initialize",	512,	NULL,	TASK_PRIORITY_HIGH, NULL);

	// Start the RTOS, this function will never return and will schedule the tasks.
	vTaskStartScheduler();

}

//=============================================================================
//                                  LOOP
//
//=============================================================================

void loop() {

	static bool flg_menu = true;
	static unsigned long loop_time = 0;
	static bool flg_led;
	static bool flg_Eth = false;
	  
	if(flg_Eth){
		 flg_Eth = EthernetLoop();			// Run until complete show of web page
		 if(!flg_Eth) flg_menu = true;		// Ethernet Client ends, show menu
		 return;
	}

	if (Serial.available() > 0) {
		char c = Serial.read();
	/*	c = c & 0xDF;	// uppercase
// If the character is a newline ("\n"), it is the last character in the incoming string. 
// Print out the string to the Console, ask for more information, and clear the string.
			if(c == 'E'){
				EthernetInit();			// Connect Ethernet
				flg_menu = true;
			}
			if(c == 'C'){	
				flg_Eth = EthernetStartClient();	// Ethernet Client -> Send request to Server
				flg_menu = true;
			}
			if(c == 'B'){
				BCloop();				// Battery charger
				flg_menu = true;
			}
			if(c == 'W'){
				WNINA_setup();				// WiFi Init UBlox NINA W102
				flg_menu = true;
			}
			if(c == 'N'){
				WNINA_connect();				// Connect to WiFi
				flg_menu = true;
			}
			if(c == 'S'){
				WNINA_scanner();				// Scanner WiFi
				flg_menu = true;
			}
			if(c == 'T'){
				ETH_test();				// Scanner WiFi
				flg_menu = true;
			} */
	}

// LED flash: 0.5 Sec ON - 0.5 Sec OFF	
	if(millis() - loop_time > 500){
	loop_time = millis();
		flg_led = !flg_led;
		if(flg_led)
			digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
		else
			digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
	}

// If necessary show menu
	if(flg_menu){
		flg_menu = false;
		//showMenu();
	}
}

void showMenu(void){
	Serial.println(F("\r\nType: E - Connect Ethernet"));
	Serial.println(F("      T - Read W5500 Register 0x39 via SPI"));	
	Serial.println(F("      C - Ethernet Client"));
	Serial.println(F("      B - Battery charger, read register via I2C"));
	Serial.println(F("      W - WiFi module UBlox NINA setup"));
	Serial.println(F("      S - Scanner WiFi"));
	Serial.println(F("      N - Connect to WiFi Network"));
	Serial.print(F("=> "));	
}

//==============================================================================
// I2C SCANNER
// Scan for peripheral on I2C bur from address 1 to 127
// The i2c_scanner uses the return value of the Write.endTransmisstion to see if
// a device did acknowledge to the address.
// Wire.cpp line 127
//==============================================================================

void i2c_scan(){
	char linea[40];	
	byte error, address;
	int nDevices;
	
	Serial.println(F("------ Scanning I2C"));
	
	nDevices = 0;
	for(address = 1; address < 128; address++ ){

		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		
		if (error == 0){
			sprintf_P(linea, PSTR("Address = %X  "), address);	// Found I2C
			Serial.print(linea);
			if(address == WiFi_NINA)	Serial.print("UBlox NINA-W102");		// Defined in variant.cpp
			if(address == PMIC_ADDRESS) Serial.print("BQ24195L Battery charger");		// Defined in variant.cpp
			Serial.println("");
			nDevices++;
		}
		else{
			if(error != 2){										// 2 = Error not found
				sprintf_P(linea, PSTR("Address = %X, ERROR = %d"), address, error);
				Serial.println(linea);
			}
		}
	}
	if (nDevices == 0)
		Serial.println(F("No I2C"));
		
	Serial.println(F("-------------------"));
}





/*
uint8_t softReset(void){
	uint16_t count=0;

	//Serial.println("Wiznet soft reset");
	// write to reset bit
	writeMR(0x80);
	// then wait for soft reset to complete
	do {
		uint8_t mr = readMR();
		//Serial.print("mr=");
		//Serial.println(mr, HEX);
		if (mr == 0) return 1;
		delay(1);
	} while (++count < 20);
	return 0;
	}
	
	*/
