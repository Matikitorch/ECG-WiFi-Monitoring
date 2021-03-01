/*
 * WIFI_NINA.cpp
 *
 * Created: 08/02/2021 17:44:07
 *  Author: Livio
 */ 


//#include "WIFI_NINA.h"

/*

// Variables
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
//char ssid[] = "SSID";        // your network SSID (name)
//char pass[] = "Password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

//int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
//char server[] = "https://www.google.com";    // name address for Google (using DNS)  //LIVIO - NON FUNZIONA
//char server[] = "http://www.google.com";    // name address for Google (using DNS)  //LIVIO - NON FUNZIONA
char server[] = "www.google.com";    // name address for Google (using DNS)  //LIVIO - FUNZIONA

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient WFclient;


//=============================================================================
// WiFi UBlox NINA W102 setup
//
// Init module WiFi NINA W102
// Try to connect to WiFi
//
// OUT: true if all OK and connected to WiFi network
//=============================================================================

bool WNINA_setup(void){

	Serial.print("\r\nWiFi NINA setup: ");
	
// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE) {
		Serial.println("Communication with WiFi module failed!");
		return false;		// return ERROR
	}

// Check for Firmware Version 
	String fv = WiFi.firmwareVersion();
	if (fv < "1.0.0") {
		Serial.println("Please upgrade the firmware");
	}
	else{
		Serial.print("WiFi firmware Version = ");
		Serial.println(fv);
	}

  // print your MAC address:
	byte mac[6];
	WiFi.macAddress(mac);
	Serial.print("MAC: ");
	printMacAddress(mac);
	
	return true;
}

//=============================================================================
// attempt to connect to WiFi network:
//=============================================================================

bool WNINA_connect(void){

	Serial.print("\r\nAttempting to connect to SSID: ");
	Serial.print(ssid);
	Serial.print(" Password ");
	Serial.println(pass);
	status = WiFi.begin(ssid, pass);		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	printResponse(status);
	if(status != WL_CONNECTED) {
		return false;
	}
	else{
		Serial.println("Connected to wifi");
		printWiFiStatus();
		return true;
	}		
}
//=============================================================================// SCANNER//=============================================================================bool WNINA_scanner(void){
	Serial.println("\r\nScanning available networks...");
	listNetworks();
}


//=============================================================================
// Send to Terminar the response
//
// typedef enum {
// 	WL_NO_SHIELD = 255,
//         WL_NO_MODULE = WL_NO_SHIELD,
//         WL_IDLE_STATUS = 0,
//         WL_NO_SSID_AVAIL,
//         WL_SCAN_COMPLETED,
//         WL_CONNECTED,
//         WL_CONNECT_FAILED,
//         WL_CONNECTION_LOST,
//         WL_DISCONNECTED,
//         WL_AP_LISTENING,
//         WL_AP_CONNECTED,
//         WL_AP_FAILED
// } wl_status_t;//=============================================================================
void printResponse(int status){
	switch(status){
		case WL_NO_MODULE:			Serial.println("WL_NO_MODULE");			break;
		case WL_IDLE_STATUS:		Serial.println("WL_IDLE_STATUS");		break;
		case WL_NO_SSID_AVAIL:		Serial.println("WL_NO_SSID_AVAIL");		break;
		case WL_SCAN_COMPLETED:		Serial.println("WL_SCAN_COMPLETED");	break;
		case WL_CONNECTED:			Serial.println("WL_CONNECTED");			break;
		case WL_CONNECT_FAILED:		Serial.println("WL_CONNECT_FAILED");	break;
		case WL_CONNECTION_LOST:	Serial.println("WL_CONNECTION_LOST");	break;
		case WL_DISCONNECTED:		Serial.println("WL_DISCONNECTED");		break;
		case WL_AP_LISTENING:		Serial.println("WL_AP_LISTENING");		break;
		case WL_AP_CONNECTED:		Serial.println("WL_AP_CONNECTED");		break;
		case WL_AP_FAILED:			Serial.println("WL_AP_FAILED");			break;
		
		default: 		
			Serial.print("Status: ");
			Serial.println(status);
			break;
	}
}


//=============================================================================
// Send to terminal WiFi parameter
//=============================================================================

void printWiFiStatus(void) {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}


void printEncryptionType(int thisType) {
	// read the encryption type and print out the name:
	switch (thisType) {
		case ENC_TYPE_WEP:
		Serial.println("WEP");
		break;
		case ENC_TYPE_TKIP:
		Serial.println("WPA");
		break;
		case ENC_TYPE_CCMP:
		Serial.println("WPA2");
		break;
		case ENC_TYPE_NONE:
		Serial.println("None");
		break;
		case ENC_TYPE_AUTO:
		Serial.println("Auto");
		break;
		case ENC_TYPE_UNKNOWN:
		default:
		Serial.println("Unknown");
		break;
	}
}


void printMacAddress(byte mac[]) {
	for (int i = 5; i >= 0; i--) {
		if (mac[i] < 16) {
			Serial.print("0");
		}
		Serial.print(mac[i], HEX);
		if (i > 0) {
			Serial.print(":");
		}
	}
	Serial.println();
}


void listNetworks(void) {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}
*/