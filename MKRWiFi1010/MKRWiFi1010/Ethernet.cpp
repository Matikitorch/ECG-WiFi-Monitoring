//=============================================================================
// 
// Ethernet
//
// Aggiungere al Progetto - Directories: D:\PROGETTI_MS\ProvaWire\MKRWiFi1010\MKRWiFi1010\ArduinoCore\Ethernet [Relativo]
// Aggiungere al Progetto - Directories: D:\PROGETTI_MS\ProvaWire\MKRWiFi1010\MKRWiFi1010\ArduinoCore\Ethernet\EthernetLarge  [Relativo]
// Replace All "inner.h" "../../../inner.h"  poi qualcuna va corretta in "../../inner.h" o in "../inner.h"
//
//=============================================================================

#include "Ethernet.h"


bool	flg_connect = false;		// true if connected 
bool	flg_EthWork = false;		// true if EthernetLoop require calls

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(54,85,55,79);  // numeric IP for Google (no DNS)
const char server[] = "www.arduino.cc";    // name address for Arduino (using DNS)
const char server_host[] = "www.arduino.cc"; // leave this alone, change only above two

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(8, 8, 8, 8);

// Choose the analog pin to get semi-random data from for SSL
// Pick a pin that's not connected or attached to a randomish voltage source
const int rand_pin = A5;

// Initialize the SSL client library
// We input an EthernetClient, our trust anchors, and the analog pin
EthernetClient base_client;
SSLClient client(base_client, TAs, (size_t)TAs_NUM, rand_pin);

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement


//=============================================================================
// Setup Ethernet connection
//=============================================================================

void EthernetInit(void){

	Serial.print("Ethernet setup: ");
// You can use Ethernet.init(pin) to configure the CS pin
   Ethernet.init(SS_ETH);   // MKR ETH shield
  
// start the Ethernet connection:
	Serial.println("Initialize Ethernet with DHCP:");
	if (Ethernet.begin(mac) == 0) {				// !!! Se il cavo è scollegato non ritorna
		Serial.println("Failed to configure Ethernet using DHCP");
// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
			return;
		}
		  
		if (Ethernet.linkStatus() == LinkOFF) {
			Serial.println("Ethernet cable is not connected.");
			return;
		}
// try to configure using IP address instead of DHCP:
		Ethernet.begin(mac, ip, myDns);
	} else {
		Serial.print("  DHCP assigned IP ");
		Serial.println(Ethernet.localIP());
	  }
	  
	  delay(2000);		// give the Ethernet shield a second to initialize:
	  flg_connect = true;
}
	  
//=============================================================================
// Ethernet Client
//
// Send request to server
//=============================================================================

bool EthernetStartClient(void){	
	if(!flg_connect){
		Serial.println("ERROR: Ethernet unavailable.");
		return false;
	}
	Serial.print("connecting to ");
	Serial.print(server);
	Serial.println("...");

// if you get a connection, report back via serial:
	auto start = millis();
// specify the server and port, 443 is the standard port for HTTPS
	if (client.connect(server, 443)) {
		auto time = millis() - start;
		Serial.print("Took: ");
		Serial.println(time);
// Make a HTTP request:
		client.println("GET /asciilogo.txt HTTP/1.1");
		client.println("User-Agent: SSLClientOverEthernet");
		client.print("Host: ");
		client.println(server_host);
		client.println("Connection: close");
		client.println();
		flg_EthWork = true;		// Need call from loop
		return true;
	} else {
		  // if you didn't get a connection to the server:
		  Serial.println("connection failed");
		  return false;
	  }
}


//=============================================================================
// Ethernet loop - Called by loop()
//
// Need after Client request to show data received from server
//
// OUT: false = operation end, no more call needed
//      true = not finish, need more call
//=============================================================================

bool EthernetLoop(void){
	if(!flg_EthWork) return(false);
	
	// if there are incoming bytes available
	// from the server, read them and print them:
	int len = client.available();
	if (len > 0) {
		byte buffer[80];
		if (len > 80) len = 80;
		client.read(buffer, len);
		if (printWebData) {
			Serial.write(buffer, len); // show in the serial monitor (slows some boards)
		}
		byteCount = byteCount + len;
	}

	// if the server's disconnected, stop the client:
	if (!client.connected()) {
		endMicros = micros();
		Serial.println();
		Serial.println("disconnecting.");
		client.stop();
		Serial.print("Received ");
		Serial.print(byteCount);
		Serial.print(" bytes in ");
		float seconds = (float)(endMicros - beginMicros) / 1000000.0;
		Serial.print(seconds, 4);
		float rate = (float)byteCount / seconds / 1000.0;
		Serial.print(", rate = ");
		Serial.print(rate);
		Serial.print(" kbytes/second");
		Serial.println();

		flg_EthWork = false;
	}
	return(true);
}

//=============================================================================
// TEST
//
// Read register 0x39 from controller Ethernet W5500
//=============================================================================

void ETH_test(void){

	uint32_t ss_pin_mask;
	uint8_t ss_pin, len;
	static volatile uint32_t *ss_pin_reg;
	uint8_t cmd[4];
	uint8_t buf[4];
	
	ss_pin = SS_ETH;

	len = 1;
	cmd[0] = 0x00;
	cmd[1] = 0x39;
	cmd[2] = 0x00;
	cmd[3] = 0x20;
	
	
	Serial.print("\r\nW5500 test: ");
// You can use Ethernet.init(pin) to configure the CS pin
	Serial.print("Pin SS: ");
	Serial.print(SS_ETH);
	Ethernet.init(SS_ETH);   // MKR ETH shield
	
	ss_pin_reg = portModeRegister(digitalPinToPort(ss_pin));
	ss_pin_mask = digitalPinToBitMask(ss_pin);
	pinMode(ss_pin, OUTPUT);
	
	SPI.begin();
	
	Serial.print("\r\nRegister: 0x39 Data Read: ");
	*(ss_pin_reg+5) = ss_pin_mask;	// Set SS
	SPI.transfer(cmd, 3);			// READ: write 3 byte and read 1
//	memset(buf, 0, len);
	SPI.transfer(buf, len);
	*(ss_pin_reg+6) = ss_pin_mask;	// Reset SS
		
	Serial.print(buf[0], HEX);
	Serial.println("");
}

