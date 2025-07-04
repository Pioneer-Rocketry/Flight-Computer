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

	// Continity Pin
	GPIO_TypeDef *continuityPort;
	uint16_t continuityPin;

	bool fired;

	Data *data;

	// Armed pins
	static GPIO_TypeDef *armedPort;
	static uint16_t armedPin;

	static bool armed;

public:
	Pyro(Data *data, GPIO_TypeDef *triggerPort, uint16_t triggerPin, GPIO_TypeDef *continuityPort, uint16_t continuityPin);

	void init();
	void update();
	void fire();

	static void set_armed_pins(GPIO_TypeDef *armedPort, uint16_t armedPin);
	static void check_armed();
};

#endif /* PYRO_H */
