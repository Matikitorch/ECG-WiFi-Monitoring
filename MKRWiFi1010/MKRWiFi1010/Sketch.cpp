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

//**************************************************************************
// global variables
//**************************************************************************
TaskHandle_t Handle_aTask;
TaskHandle_t Handle_bTask;
TaskHandle_t Handle_monitorTask;

//**************************************************************************
// Can use these function for RTOS delays
// Takes into account processor speed
// Use these instead of delay(...) in rtos tasks											
//**************************************************************************
void myDelayUs(int us)
{
  vTaskDelay( us / portTICK_PERIOD_US );  
}

void myDelayMs(int ms)
{
  vTaskDelay( (ms * 1000) / portTICK_PERIOD_US );  
}

void myDelayMsUntil(TickType_t *previousWakeTime, int ms)
{
  vTaskDelayUntil( previousWakeTime, (ms * 1000) / portTICK_PERIOD_US );  
}


//*****************************************************************
// Create a thread that prints out A to the screen every two seconds
// this task will delete its self after printing out afew messages
//*****************************************************************
static void threadA( void *pvParameters ) 
{
  
  Serial.println("Thread A: Started");
  for(int x=0; x<20; ++x)
  {
    Serial.println("A");
    myDelayMs(500);
  }
  
  // delete ourselves.
  // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
  Serial.println("Thread A: Deleting");
  vTaskDelete( NULL );
}

//*****************************************************************
// Create a thread that prints out B to the screen every second
// this task will run forever
//*****************************************************************
static void threadB( void *pvParameters ) 
{
  Serial.println("Thread B: Started");

  while(1)
  {
    Serial.println("B");
    myDelayMs(2000);
  }

}

//*****************************************************************
// Task will periodically print out useful information about the tasks running
// Is a useful tool to help figure out stack sizes being used
// Run time stats are generated from all task timing collected since startup
// No easy way yet to clear the run time stats yet
//*****************************************************************
static char ptrTaskList[400]; //temporary string buffer for task stats

void taskMonitor(void *pvParameters)
{
    int x;
    int measurement;
    
    Serial.println("Task Monitor: Started");

    // run this task afew times before exiting forever
    for(x=0; x<10; ++x)
    {
  
		Serial.println("");
    	Serial.println("****************************************************");
    	Serial.println("Free Heap: ");
    	Serial.println(xPortGetFreeHeapSize());
    	Serial.println(" bytes");

    	Serial.print("Min Heap: ");
    	Serial.println(xPortGetMinimumEverFreeHeapSize());
    	Serial.println(" bytes");
		
		
    	Serial.println("****************************************************");
    	Serial.println("Task            ABS             %Util");
    	Serial.println("****************************************************");

    	vTaskGetRunTimeStats(ptrTaskList); //save stats to char array
    	Serial.println(ptrTaskList); //prints out already formatted stats

		Serial.println("****************************************************");
		Serial.println("Task            State   Prio    Stack   Num     Core" );
		Serial.println("****************************************************");

		vTaskList(ptrTaskList); //save stats to char array
		Serial.println(ptrTaskList); //prints out already formatted stats

		Serial.println("****************************************************");
		Serial.println("[Stacks Free Bytes Remaining] ");

		measurement = uxTaskGetStackHighWaterMark( Handle_aTask );
		Serial.println("Thread A: ");
		Serial.println(measurement);

		measurement = uxTaskGetStackHighWaterMark( Handle_bTask );
		Serial.println("Thread B: ");
		Serial.println(measurement);

		measurement = uxTaskGetStackHighWaterMark( Handle_monitorTask );
		Serial.println("Monitor Stack: ");
		Serial.println(measurement);

		Serial.println("****************************************************");


      myDelayMs(10000); // print every 10 seconds
    }

    // delete ourselves.
    // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
    Serial.println("Task Monitor: Deleting");
    vTaskDelete( NULL );

}



//=============================================================================
//                                 SETUP
//
//=============================================================================

void setup() {

	pinMode(LED_BUILTIN, OUTPUT);	// initialize digital pin LED_BUILTIN as an output.
	
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
	Serial.println(F("    Test MKRWiFi1010 - Microchip Studio"));
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
	
// Ethernet
//	EthernetInit();	
	
	
	Serial.println(F("================== END SETUP =================\r\n"));
	
	
	// Create the threads that will be managed by the rtos
	// Sets the stack size and priority of each task
	// Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
	//xTaskCreate(threadA,     "Task A",       256, NULL, tskIDLE_PRIORITY + 3, &Handle_aTask);
	//xTaskCreate(threadB,     "Task B",       256, NULL, tskIDLE_PRIORITY + 2, &Handle_bTask);
	//xTaskCreate(taskMonitor, "Task Monitor", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);
	
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
		c = c & 0xDF;	// uppercase
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
			}
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
		showMenu();
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
