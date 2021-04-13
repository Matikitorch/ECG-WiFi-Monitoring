/*
 * temp.h
 */ 

#ifndef TEMP_H_
#define TEMP_H_

#include <stdlib.h>

/*
 * Initializes the Temp
 */
void task_TempInitialize(void *pvParameters);


//********************************************************
//
// float getTemp()
//
// Author: Justin Bee
// Date: 3/27/2021
// Returns the value of tempF
//********************************************************
float getTemp();

#endif /* TEMP_H_ */