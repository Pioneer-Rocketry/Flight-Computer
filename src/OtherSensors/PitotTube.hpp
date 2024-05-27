// OtherSensors/PitotTube.hpp
#ifndef PITOT_TUBE_hpp
#define PITOT_TUBE_hpp

#include <Arduino.h>
#include <Wire.h>

#include "Memory.h"
#include "Data.hpp"
#include "Sensor.hpp"

// Based on this sketchy function from 2016 competition code
// https://github.com/oberhauserg/PioneerRocketry/blob/master/2016-Competition-AirBrakes/Adam/BetterSketch/Sensors.ino#L17
class PitotTube : Sensor {
  private:
		float airspeed;
		float value;

		int pin = A2; // Analog pin for pitot tube

    void update_sensor() {
			// Read the analog value from the pitot tube
			value = analogRead(pin);

			// Convert the analog value to airspeed
			airspeed = value / 1024.; // I don't know is this is correct but IDK stolen from 2016 code
		}

	public:

    void get_data(Data *data) {
			data->air_speed = airspeed;
		}

    bool begin() {
			pinMode(pin, INPUT);

			return true;
		}

		PitotTube() : Sensor(0x00) { // no address for pitot tube so 0x00
			
		}
};

#endif