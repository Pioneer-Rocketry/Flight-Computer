#include "Pyro.h"

Pyro::Pyro(Data *data, GPIO_TypeDef *triggerPort, uint16_t triggerPin, GPIO_TypeDef *continuityPort, uint16_t continuityPin) {
	// armed = false;

	this->data = data;
	this->triggerPort = triggerPort;
	this->triggerPin = triggerPin;
	this->continuityPort = continuityPort;
	this->continuityPin = continuityPin;
}

void Pyro::init() {

}

void Pyro::update() {
	// ADC read the voltage
}

void Pyro::fire() {
	// Trigger the pyro
}

void Pyro::set_armed_pins(GPIO_TypeDef *armedPort, uint16_t armedPin) {
	armedPort = armedPort;
	armedPin = armedPin;
}

void Pyro::check_armed() {
	// ADC read the voltage
}