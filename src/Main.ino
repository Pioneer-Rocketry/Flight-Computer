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
#include "Radio/Radio.hpp"

#include "KalmanFilter.h"
#include <MadgwickAHRS.h>

#define RECORD_BUTTON 14
bool recording = false;

BNO055 acc;
LPS25HB baro;
GPS gps;
PitotTube pitot;

Memory memory;
// Radio radio;
State_Machine stateMachine;
Fin_Controller fins;

PyroChannel channel1(1);
PyroChannel channel2(2);
PyroChannel channel3(3, TestCondition);
PyroChannel channel4(4);

KalmanFilter kalmanFilter;
Madgwick filter;

unsigned long loopFreq   = 100; // In Hz
unsigned long loopLenght = 1000 / loopFreq; // In ms
unsigned long loopDelay  = 0;
unsigned long loopStart  = 0;
unsigned long timeStart  = 0;

unsigned long start = 0;
unsigned long end = 0;

Data data;

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

  // Start Madgwick Filter
  filter.begin(loopFreq);

  // State Radio
  // radio.begin();

  data.dt = loopFreq;
  kalmanFilter.init(&data);

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
      acc.get_data(&data);
      baro.get_data(&data);
      gps.get_data(&data);
      // pitot.get_data(&data);

      // Update the pyro channels
      // channel1.update(&data);
      // channel2.update(&data);
      // channel3.update(&data);
      // channel4.update(&data);

      // Run the Kalman Filter
      kalmanFilter.run(&data); // Run the Kalman Filter

      // Run the Madgwick Filter
      // filter.updateIMU(
      //   data.gyrX, data.gyrY, data.gyrZ,
      //   data.accX, data.accY, data.accZ
      //   // data.magX, data.magY, data.magZ
      // );

      // data.pitch = filter.getPitch();
      // data.roll = filter.getRoll();
      // data.yaw = filter.getYaw();

      // Update the state machine
      stateMachine.update(&data);

      // radio.send_data(&data);

      // Write data to memory
      memory.write_data(&data);

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
  acc.get_data(&data);

  // Read the starting alt for the barometer
  baro.get_data(&data);
  data.starting_alt = data.alt;

  kalmanFilter.init(&data);

  // Print the header
  Serial.println(memory.header);
  memory.write_header();
}