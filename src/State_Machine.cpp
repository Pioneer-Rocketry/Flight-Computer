// State_Machine.cpp
#include "State_Machine.h"

State_Machine::State_Machine(int num_stages) {
  state = on_launch_rail;

  this->stagesRemaining = num_stages;
  
  for (int i=0; i < numOfAltSamples; i++) {
    lastAltitude[i] = 0;
  }
}

State State_Machine::update(Data *data) {
  switch (state) {
    case on_launch_rail: switch_to_powered_flight(data);    break;
    case powered_flight: switch_to_unpowered_flight(data);  break;
    
    case unpowered_flight:
      switch_to_descent(data);

      // If there is another stage, and we are not in the descent state
      if (stagesRemaining >  1 && state != descent)
        switch_to_powered_flight(data);
      break;

    case descent:
      switch_to_parachute_descent(data);
      
      // Need to catch if there is no parachute deployed and we crash into the ground
      if (state != parachute_descent) switch_to_landed(data);
      
      break;
    case parachute_descent: switch_to_landed(data);             break;
    case landed:            switch_to_powered_flight(data);     break;

    default: break;
  }

  // Update the last altitude
  for (int i = numOfAltSamples-1; i > 0; i--) {
    lastAltitude[i] = lastAltitude[i - 1];
  }
  lastAltitude[0] = data->alt;

  // Log the state
  data->state = (float)state;

  return state;
}

// The rocket engine has ignited and the rocket is accelerating upwards
void State_Machine::switch_to_powered_flight(Data *data) {
  // Conditions: vertical acceleration > 5.0 m/s^2
  //    cause on the launch rail SHOULD be 0 m/s^2, and in the air coasting (if mulitstage) SHOULD be -9.8 m/s^2 (or less)
  if (data->accZ > 5.0) {
    state = powered_flight;
  }
}

// The rocket engine has burned out and the rocket is coasting upwards
void State_Machine::switch_to_unpowered_flight(Data *data) {
  // Conditions: vertical acceleration < -9 m/s^2
  if (data->accZ < -9.0) {
    state = unpowered_flight;

    stagesRemaining--;
  }
}

// The rocket has passed the apogee and is descending
void State_Machine::switch_to_descent(Data *data) {
  // Conditions: altitude is less than the previous altitude (check over a 1 second period)
  
  // Check loop over the last n altitudes
  for (int i = 0; i < numOfAltSamples; i++) {
    // Check if the altitude is less than the previous altitude
    if (data->alt > lastAltitude[i]) { 
      return; // If its greater, return
    }
  }

  state = descent;
}

// The parachute has been deployed and the rocket is descending
void State_Machine::switch_to_parachute_descent(Data *data) {
  // Conditions: If vertical velocity is 
}

// The rocket has landed on the ground
void State_Machine::switch_to_landed(Data *data) {
  // Conditions: If the last n number of altitudes are with in +- 0.5 m

  // Check loop over the last n altitudes
  for (int i = 0; i < numOfAltSamples; i++) {
    // Check if the altitude is less than the previous altitude
    if (abs(data->alt - lastAltitude[i]) > 0.5) { 
      return; // If its greater, return
    }
  }

  state = landed;
}