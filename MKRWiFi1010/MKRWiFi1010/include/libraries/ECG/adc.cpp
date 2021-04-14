/*
 * adc.cpp
 */ 

#include <stdlib.h>

#include <WiFiNINA.h>

#include "FreeRTOS_SAMD21.h"

#include "hdr.h"

#define IRQ_PRIORITY_LOW	(3)
#define IRQ_PRIORITY_MED	(2)
#define IRQ_PRIORITY_HIGH	(1)

#define IRQ_SAMPLE_RATE		(1000)

volatile uint32_t aout = 0;
volatile uint32_t ain = 0;

/*
 * Gets called by the interrupt at IRQ_SAMPLE_RATE rate
 */
void TC5_Handler(void)
{
	static uint32_t toggle;
	
	ain = analogRead(A1); // 0 == 0V -> 1023 == 3.3V	
	analogWrite(A0, ain); // 0 == 0V -> 1023 == 3.3V
	
	digitalWrite(A5, toggle);
	toggle ^= 1;
	
	TC5->COUNT16.INTFLAG.bit.MC0 = 1; //Writing a 1 to INTFLAG.bit.MC0 clears the interrupt so that it will run again
}

/*
 * Function that is used to check if TC5 is done syncing
 * returns true when it is done syncing
 */
int tcIsSyncing()
{
	return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

/*
 * This function enables TC5 and waits for it to be ready
 */
void tcStartCounter()
{
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
	while (tcIsSyncing()); //wait until synchronized
}

/*
 * Reset TC5
 */
void tcReset()
{
	TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	while (tcIsSyncing());
	while (TC5->COUNT16.CTRLA.bit.SWRST);
}

/*
 * Disable TC5
 */
void tcDisable()
{
	TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

/*
 * Configures the timer counter interrupt
 */
void tcConfigure()
{
	// select the generic clock generator used as source to the generic clock multiplexer
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
	while (GCLK->STATUS.bit.SYNCBUSY);

	tcReset(); //reset TC5

	// Set Timer counter 5 Mode to 16 bits, it will become a 16bit counter ('mode1' in the data sheet)
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
	
	// Set TC5 waveform generation mode to 'match frequency'
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
	
	//set prescaler
	//the clock normally counts at the GCLK_TC frequency, but we can set it to divide that frequency to slow it down
	//you can use different prescaler division here like TC_CTRLA_PRESCALER_DIV1 to get a different range
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_ENABLE; //it will divide GCLK_TC frequency by 1024
	
	//set the compare-capture register.
	//The counter will count up to this value (it's a 16bit counter so we use uint16_t)
	//this is how we fine-tune the frequency, make it count to a lower or higher value
	//system clock should be 1MHz (8MHz/8) at Reset by default
	TC5->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock / IRQ_SAMPLE_RATE / 1000);
	while (tcIsSyncing());
	
	// Configure interrupt request
	NVIC_DisableIRQ(TC5_IRQn);
	NVIC_ClearPendingIRQ(TC5_IRQn);
	NVIC_SetPriority(TC5_IRQn, IRQ_PRIORITY_MED);
	NVIC_EnableIRQ(TC5_IRQn);

	// Enable the TC5 interrupt request
	TC5->COUNT16.INTENSET.bit.MC0 = 1;
	while (tcIsSyncing()); //wait until TC5 is done syncing
}

/*
 * Initializes the ADC
 */
void task_ADCInitialize(void *pvParameters)
{	
	//analogOutputInit();
	
	tcConfigure();
	tcStartCounter();
	
	vTaskDelete(NULL);
}


uint32_t getAin()
{
	return ain;
}