/*
 * adc.h
 *
 * Created: 2/18/2021 10:01:02 PM
 *  Author: m.blanc
 */ 


#ifndef ADC_H_
#define ADC_H_

/*
 * Initializes the ADC
 */
void task_ADCInitialize( void *pvParameters );


uint32_t getAin();

#endif /* ADC_H_ */