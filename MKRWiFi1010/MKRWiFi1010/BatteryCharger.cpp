//=============================================================================
// BatteryCharger.cpp
//
// MKRWiFi1010: BQ24195L Battery charger
// See: https://www.ti.com/product/BQ24195L
// if USE_BQ24195L_PMIC is defined in Project Settings main() -> initVariant()
// will initialize the battery control chip via I2C
//
//=============================================================================

#include "BatteryCharger.h"



void BCsetup(void){
	
// Already done in initVariant()	
}


bool BCloop(void){
	
	static unsigned long BC_time = 0;
	
// I2C: Show register of BQ24195L Battery charger
		Serial.print("\r\n\r\nBQ24195L Register:");
	
		Wire.beginTransmission(PMIC_ADDRESS);
		Wire.write(0x00);						// Select REG00
		Wire.endTransmission();
		Wire.requestFrom(PMIC_ADDRESS, 10);		// Address, byte quantity requested

		while(Wire.available())					// slave may send less than requested
		{
			char c = Wire.read();				// receive a byte as character
			Serial.print(c < 16 ? "0" : "");
			Serial.print(c, HEX);				// print the character in hexadecimal
			Serial.print(" ");
		}
		Serial.println("");
		return true;
}