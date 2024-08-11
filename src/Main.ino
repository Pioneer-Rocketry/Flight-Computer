#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

#include "ColorLED.hpp"
#include "State_Machine.h"
#include "Buzzer.hpp"
#include "Memory.h"
#include "Data.hpp"
#include "Pyro.hpp"
#include "PyroConditions.hpp"
#include "Fin_Controller.hpp"

#include "Accelerometers/BNO055.h"
#include "Barometers/LPS25HB.h"
#include "GPS.h"
#include "OtherSensors/PitotTube.hpp"

#include "KalmanFilter.h"

#define RECORD_BUTTON 14
bool recording = false;

Data data;

BNO055 acc(&data);
LPS25HB baro(&data);
GPS gps(&data);
PitotTube pitot(&data);

Memory memory(&data);
State_Machine stateMachine(&data);
Fin_Controller fins(&data);

PyroChannel channel1(1, &data);
PyroChannel channel2(2, &data);
PyroChannel channel3(3, &data, TestCondition);
PyroChannel channel4(4, &data);

KalmanFilter kalmanFilter(&data);

unsigned long loopFreq   = 30; // In Hz
unsigned long loopLenght = 1000 / loopFreq; // In ms
unsigned long loopDelay  = 0;
unsigned long loopStart  = 0;
unsigned long timeStart  = 0;

unsigned long start = 0;
unsigned long end = 0;

void setup() {
	Serial.begin(115200);

	delay(500);
	Serial.println("Setup started");

	Wire.begin();
	
	ColorLED::begin(); // Commented because the LED is very bright
	// Buzzer::begin();
	ColorLED::show_blue();

	pinMode(RECORD_BUTTON, INPUT);

	// Get the sensors online
	// Start Accelerometer
	if (!acc.begin()) {
		Serial.println("Accelerometer not online");
		error();
	}
	Serial.println("Accelerometer online");
	acc.get_calibration();
	// acc.write_calibrations();
	// acc.get_calibration_offsets();

	// Start Barometer
	if (!baro.begin()) {
		Serial.println("Barometer not online");
		error();
	}
	Serial.println("Barometer online");

	// Start GPS
	if (!gps.begin()) {
		Serial.println("GPS not online");
		error();
	}
	Serial.println("GPS online");

	// Start Memory
	if (!memory.begin()) {
		Serial.println("SPI Flash not online");
		error();
	}
	Serial.println("SPI Flash detected.");

	// Start Pitot Tube
	// if (!pitot.begin()) {
	//   Serial.println("Pitot Tube not online");
	//   error();
	// }
	// Serial.println("Pitot Tube online");

	// fins.begin();
	// fins.sweep();
	// Serial.println("Fins setup");

	// Start Pyro Channels
	channel1.begin();
	channel2.begin();
	channel3.begin();
	channel4.begin();
	// Serial.println("Pyro Channels setup");
	
	data.dt = loopFreq;
	kalmanFilter.init(); // Run the Kalman Filter setup step

	// Setup Finished

	// Show good status
	ColorLED::show_green();
	// Buzzer::start_up_sound();

	Serial.println("Setup finished, everything is good.");
}

void loop() {
	// check if the pushbutton is pressed. If it is, the buttonState is HIGH:
	if (digitalRead(RECORD_BUTTON) == HIGH) {
		start_recording();
	}

	if (recording) {
		ColorLED::show_red();

		if ((millis() - loopStart) >= loopLenght) {
			loopStart = millis();

			// Record data
			data.time = (millis()-timeStart) / 1000.0f;
			
			// Update the sensors
			acc.get_data();
			baro.get_data();
			gps.get_data();
			// pitot.get_data();

			// Update the pyro channels
			// channel1.update();
			// channel2.update();
			// channel3.update();
			// channel4.update();

			// Run the Kalman Filter
			kalmanFilter.run(); // Run the Kalman Filter

			// Update the state machine
			stateMachine.update();

			// Write data to memory
			memory.write_data();

			// Print data
			Serial.println(data.get_data());
		}
	} else {
		if (Serial.available() != 0) {
			char choice = Serial.read(); 

			switch (choice) {
				case 'e': memory.erase_data(); break; // Erase data
				case 'd': memory.dump_to_sd(); break; // Dump data to SD card
				case 'p': memory.print();      break; // Print data
				case 'r': start_recording();   break; // Start recording

				default: break;
			}
		}
	}
}

void error() {
	while (1) {
		Buzzer::error_sound();
		ColorLED::flash_red();
	}
}

void start_recording() {
	if (!recording)
		ColorLED::show_red();
		// Buzzer::countdown(1);
	else 
		ColorLED::show_green();

	recording = !recording;

	delay(500);
	timeStart = millis(); // Reset the time

	// Reset the accelerometer drived data
	acc.reset();
	acc.get_data();

	// Read the starting alt for the barometer
	baro.get_data();
	data.starting_alt = data.alt;

	kalmanFilter.init();

	// Print the header
	Serial.println(memory.header);
	memory.write_header();
}