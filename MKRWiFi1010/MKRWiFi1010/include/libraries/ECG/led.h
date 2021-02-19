/*
 * led.h
 *
 * Created: 2/18/2021 10:00:51 PM
 *  Author: m.blanc
 */ 


#ifndef LED_H_
#define LED_H_

typedef enum _led_mode
{
	led_mode_starting,
	led_mode_connecting,
	led_mode_normal,
	led_mode_faulted
} led_mode_t;

/* 
 * Sets the LED in a temporary mode for X-amount of time
 */
void led_modeTemp(led_mode_t mode, uint32_t ms);

/*
 * Set the LED in a new permanent mode
 */
void led_mode(led_mode_t mode);
	
/*
 * Initializes the LEDs
 */
void task_LEDInitialize( void *pvParameters );

#endif /* LED_H_ */