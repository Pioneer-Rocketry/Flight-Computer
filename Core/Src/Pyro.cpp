#include "Pyro.h"
#include "stm32f4xx_hal.h"

Pyro::Pyro(Data *data, GPIO_TypeDef *triggerPort, uint16_t triggerPin, ADC_HandleTypeDef continuityADC) {
	// armed = false;

	this->data = data;
	this->triggerPort = triggerPort;
	this->triggerPin = triggerPin;
	this->continuityADC = continuityADC;
}

void Pyro::init() {

	// Calibrate the ADC channel
	// HAL_ADCEx_Calibration_Start(&continuityADC, ADC_SINGLE_ENDED);

}

void Pyro::update() {
	// ADC read the voltage

	HAL_ADC_Start(&continuityADC);
	HAL_ADC_PollForConversion(&continuityADC, 1);

	cont_volt = HAL_ADC_GetValue(&continuityADC);

	// The continuity check is a bit more complex than just checking if the ADC voltage isn't 0
	// You have to figure out the disconnected voltage which is dependent on the armed ADC voltage
}

void Pyro::fire() {
	// Trigger the pyro
}

void Pyro::set_armed_pins(ADC_HandleTypeDef armedADC) {
	armedADC = armedADC;
}

void Pyro::check_armed() {
	// ADC read the voltage
}