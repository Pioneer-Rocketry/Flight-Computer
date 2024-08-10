// State_Machine.h
#ifndef STATE_MACHINE_h
#define STATE_MACHINE_h

#include <Arduino.h>

#include "Memory.h"
#include "Data.hpp"

enum State {
	on_launch_rail,     // On the launch rail waiting for the launch
	powered_flight,     // The engine is burning and the rocket is accelerating upwards
	unpowered_flight,   // The engine has burned out and the rocket is coasting upwards
	descent,            // The rocket has passed the apogee and is descending
	parachute_descent,  // The parachute has been deployed and the rocket is descending
	landed              // The rocket has landed on the ground
};

const int numOfAltSamples = 10;
class State_Machine {
	private:
		State state;
		int stagesRemaining; // The number of stages the rocket has

		float lastAltitude[numOfAltSamples]; // The altitude of the rocket in the last loop

		// Conditions
		void switch_to_powered_flight(Data *data);
		void switch_to_unpowered_flight(Data *data);
		void switch_to_descent(Data *data);
		void switch_to_parachute_descent(Data *data);
		void switch_to_landed(Data *data);

	public:
		State_Machine(int num_stages=1);

		State update(Data *data);
};

#endif