/*
 * Sketch.h
 *
 * Created: 08/02/2021 08:45:13
 *  Author: Livio
 */ 


#ifndef SKETCH_H_
#define SKETCH_H_

#include <Arduino.h>
#include <Wire.h>
#include <MCP23017.h>
#include <SPI.h>

#include "Definitions.h"
#include "Ethernet.h"
#include "BatteryCharger.h"
#include "WIFI_NINA.h"


// Function prototypes
void setup();
void loop();
void showMenu(void);
void i2c_scan();




#endif /* SKETCH_H_ */