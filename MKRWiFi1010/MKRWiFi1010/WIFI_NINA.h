/*
 * WIFI_NINA.h
 *
 * Created: 08/02/2021 17:44:23
 *  Author: Livio
 */ 


#ifndef WIFI_NINA_H_
#define WIFI_NINA_H_

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <utility/wl_definitions.h>		// I need this to have error definitions

#include "arduino_secrets.h"			// Id and Passwords


bool WNINA_setup(void);
bool WNINA_connect(void);
bool WNINA_scanner(void);
void printWiFiStatus();
void printResponse(int);

void printEncryptionType(int thisType);
void printMacAddress(byte mac[]);
void listNetworks(void);

#endif /* WIFI_NINA_H_ */