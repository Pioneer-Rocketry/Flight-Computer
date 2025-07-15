#ifndef PYRO_H
#define PYRO_H

#include "stm32f4xx_hal.h"

#include "data.h"


class Pyro
{
private:

	// Trigger Pin
	GPIO_TypeDef *triggerPort;
	uint16_t triggerPin;

	// Continity
	ADC_HandleTypeDef continuityADC;

	float cont_volt;
	bool fired;
	Data *data;

	// Armed
	static ADC_HandleTypeDef armedADC;
	static float pyro_volt;
	static bool armed;

public:
	Pyro(Data *data, GPIO_TypeDef *triggerPort, uint16_t triggerPin, ADC_HandleTypeDef continuityADC);

	void init();
	void update();
	void fire();

	static void set_armed_pins(ADC_HandleTypeDef armedADC);
	static void check_armed();
};

#endif /* PYRO_H */
