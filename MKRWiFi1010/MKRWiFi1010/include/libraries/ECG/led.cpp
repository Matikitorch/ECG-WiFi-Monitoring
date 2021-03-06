/*
 * led.c
 *
 * Created: 2/18/2021 10:00:43 PM
 *  Author: m.blanc
 */ 

#include <stdlib.h>

#include <WiFiNINA.h>

#include "FreeRTOS_SAMD21.h"

#include "hdr.h"
#include "led.h"

#define COLOR_LED_OFF	0
#define COLOR_LED_RED	1
#define COLOR_LED_GRN	2
#define COLOR_LED_BLU	3

/*
 * Structure that holds the LED light time to be on and off.
 */
typedef struct
{
	uint32_t Time;				// Keeps track of the time
	uint32_t EndTime; 			// Only a temporary LED state uses EndTime

	uint32_t RedBlinks;			// Number of RED blinks
	uint32_t GrnBlinks;			// Number of GRN blinks

	uint32_t RedOnTime;			// RED on time
	uint32_t RedOffTime;		// RED off time
	uint32_t GrnOnTime;			// GRN on time
	uint32_t GrnOffTime;		// GRN off time
} led_state_t, *pled_state_t;

led_mode_t oldMode;
led_mode_t currentMode;

led_state_t currentState;
led_state_t temporaryState;

void setLEDColor(int color)
{
	if(color == COLOR_LED_RED)
	{
		WiFi.LEDRed(1);
		WiFi.LEDGreen(0);
		WiFi.LEDBlue(0);
	}
	else if(color == COLOR_LED_GRN)
	{
		WiFi.LEDRed(0);
		WiFi.LEDGreen(1);
		WiFi.LEDBlue(0);
	}
	else if(color == COLOR_LED_BLU)
	{
		WiFi.LEDRed(0);
		WiFi.LEDGreen(0);
		WiFi.LEDBlue(1);
	}
	else
	{
		WiFi.LEDRed(0);
		WiFi.LEDGreen(0);
		WiFi.LEDBlue(0);
	}
}

/*
 * Given an LED mode, returns the amount of time that the LED is on and off.
 *
 * Order of events:
 * 1. If RedBlinks is 0 then skip to #5
 * 2. RED LED will turn on for RedOnTime
 * 3. RED LED will turn off for RedOffTime
 * 4. If RedBlinks is greater than 0 then repeat #2 thru #4 until it is 0
 * 5. If GreenBlinks is 0 then skip to #9
 * 6. GRN LED will turn on for GrnOnTime
 * 7. GRN LED will turn off for GrnOffTime
 * 8. If GrnBlinks is greater than 0 then repeat #6 thru #8 until it is 0
 * 9. Repeat (Go to #1)
 */
led_state_t GetLEDState(led_mode_t mode)
{
	led_state_t state;

	switch(mode)
	{
		case led_mode_starting:
			state.RedBlinks     =    1;     state.GrnBlinks     =    0;
			state.RedOnTime     =  100;     state.GrnOnTime     =    0;
			state.RedOffTime    =  900;     state.GrnOffTime    =    0;
			break;
			
			case led_mode_connecting:
			state.RedBlinks     =    1;     state.GrnBlinks     =    1;
			state.RedOnTime     =  125;     state.GrnOnTime     =  125;
			state.RedOffTime    =   50;     state.GrnOffTime    =   50;
			break;

			case led_mode_normal:
			state.RedBlinks     =    0;     state.GrnBlinks     =    1;
			state.RedOnTime     =    0;     state.GrnOnTime     =  500;
			state.RedOffTime    =    0;     state.GrnOffTime    =    0;
			break;

			case led_mode_faulted:
			default:
			state.RedBlinks     =    1;     state.GrnBlinks     =    0;
			state.RedOnTime     =  450;     state.GrnOnTime     =    0;
			state.RedOffTime    =   50;     state.GrnOffTime    =    0;
			break;
	}

	state.RedOnTime = pdMS_TO_TICKS(state.RedOnTime);
	state.RedOffTime = pdMS_TO_TICKS(state.RedOffTime);
	state.GrnOnTime = pdMS_TO_TICKS(state.GrnOnTime);
	state.GrnOffTime = pdMS_TO_TICKS(state.GrnOffTime);

	return state;
}

/*
 * Saves the current LED mode and sets a temporary LED state for an X amount of time.
 *
 * The LED thread looks for temporaryState.Time. If we already have a temporary state
 * going on, then nothing will happen.
 */
void led_modeTemp(led_mode_t mode, uint32_t ms)
{
	// Check if we have a temporary state already, if we do, then ignore this call
	if(temporaryState.Time == 0)
	{
		// Save the current mode globally so we can go back to it
		oldMode = currentMode;

		// Overrides any other temporary state
		temporaryState = GetLEDState(mode);
		temporaryState.Time = pdMS_TO_TICKS(ms);
		temporaryState.EndTime = 0;

		currentMode = mode;
	}

	return;
}

/*
 * Sets the permanent LED mode.
 */
void led_mode(led_mode_t mode)
{
	// If we don't have a temporary state, then set the LED state to what we want now
	if(temporaryState.Time == 0)
	{
		currentMode = mode;
	}
	else
	{
		oldMode = mode;
	}

	currentState = GetLEDState(mode);
}

/*
 * Task that is responsible for blinking the LED.
 *
 * If it sees that it has a temporary LED mode, then this will be taken care of
 * first. Otherwise the thread will turn the LED on/off and sleep until
 * the LED has to change again.
 *
 * NOTE: One LED is taken care of at a time.
 */
void task_ledRun(void *pvParameters)
{
	int i;
	int rollover;
	uint32_t tick;
	pled_state_t state;

	rollover = 0;
	temporaryState.Time = 0;
	temporaryState.EndTime = 0;
	
    for(;;)
    {
    	// If we want a temporary LED state, hit here
    	if(temporaryState.Time > 0)
    	{
    		state = &temporaryState;

			// Get the current tick count
			tick = xTaskGetTickCount();

			// If it is the first time we are running this loop with a temporary state, find the ending time
			if(state->EndTime == 0)
			{
				state->EndTime = tick + state->Time;

				// Check for rollover
				if(tick > state->EndTime)
				{
					rollover = 1;
				}
				else
				{
					rollover = 0;
				}
			}

			// Rollover loop
			if(rollover > 0)
			{
				// The millisecond tick rolls over, reset the rollover crudity variable
				if(tick < state->EndTime)
				{
					rollover = 0;
				}
			}
			// Once we reach the ending time, reset the LED to the way it was before
			else if(tick > state->EndTime)
			{
				state->Time = 0;
				currentMode = oldMode;

				state = &currentState;
			}
    	}
    	// Run normally
    	else
    	{
    		state = &currentState;
    	}

    	// If we don't have an LED state, sleep some here
    	if((state->RedBlinks == 0) && (state->GrnBlinks == 0))
    	{
    		vTaskDelay(10);
    	}
    	else
    	{
    		// Handle red blinks
    		for(i = 0; i < state->RedBlinks; i++)
    		{
    			if(state->RedOnTime > 0)
    			{
					setLEDColor(COLOR_LED_RED);
    				vTaskDelay(state->RedOnTime);
    			}

    			if(state->RedOffTime > 0)
    			{
					setLEDColor(COLOR_LED_OFF);
    				vTaskDelay(state->RedOffTime);
    			}
    		}

    		// Handle green blinks
    		for(i = 0; i < state->GrnBlinks; i++)
    		{
    			if(state->GrnOnTime > 0)
    			{
    				setLEDColor(COLOR_LED_GRN);
    				vTaskDelay(state->GrnOnTime);
    			}

    			if(state->GrnOffTime > 0)
    			{
					setLEDColor(COLOR_LED_OFF);
    				vTaskDelay(state->GrnOffTime);
    			}
    		}
    	}
    }

	vTaskDelete(NULL);
}

void task_LEDInitialize(void *pvParameters)
{
	WiFi.LEDInit();
	
	// Set the LED modes
	currentMode = led_mode_starting;
	oldMode = led_mode_starting;
	
	// Set the LED states
	currentState = GetLEDState(currentMode);
	temporaryState.Time = 0;
	
	// Start task
	xTaskCreate(task_ledRun,			"LED Run",			256,	NULL,	TASK_PRIORITY_LOW,	NULL);

	vTaskDelete(NULL);
}