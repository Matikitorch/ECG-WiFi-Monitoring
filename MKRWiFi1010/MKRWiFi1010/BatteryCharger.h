/*
 * BatteryCharger.h
 *
 * Created: 08/02/2021 14:40:13
 *  Author: Livio
 */ 


#ifndef BATTERYCHARGER_H_
#define BATTERYCHARGER_H_

#include <Arduino.h>
#include "Definitions.h"
#include <Wire.h>




void BCsetup(void);
bool BCloop(void);



#endif /* BATTERYCHARGER_H_ */