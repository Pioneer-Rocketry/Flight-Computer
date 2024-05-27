// Pyro.hpp
#ifndef PYRO_h
#define PYRO_h

#include <Arduino.h>

#include "Memory.h"

#define CHANNEL_1_TRIGGER_PIN 38
#define CHANNEL_2_TRIGGER_PIN 40
#define CHANNEL_3_TRIGGER_PIN 27
#define CHANNEL_4_TRIGGER_PIN 25

#define CHANNEL_1_STATUS_PIN 39
#define CHANNEL_2_STATUS_PIN 41
#define CHANNEL_3_STATUS_PIN 26
#define CHANNEL_4_STATUS_PIN 24

#define PYRO_ARM_PIN 23

// Used to control the pyro channels (IC: VN5E160S-E)
class PyroChannel {
  private:
    int triggerPin;
    int statusPin;

    int armedPin;
    int channelNum;

    bool fired = false;
    bool (*condition)(Data*);

  public:
    PyroChannel(int channelNum, bool (*condition)(Data*)=NULL) {
      switch (channelNum) {
        case 1:
          this->triggerPin = CHANNEL_1_TRIGGER_PIN;
          this->statusPin = CHANNEL_1_STATUS_PIN;
          break;

        case 2:
          this->triggerPin = CHANNEL_2_TRIGGER_PIN;
          this->statusPin = CHANNEL_2_STATUS_PIN;
          break;

        case 3:
          this->triggerPin = CHANNEL_3_TRIGGER_PIN;
          this->statusPin = CHANNEL_3_STATUS_PIN;
          break;

        case 4:
          this->triggerPin = CHANNEL_4_TRIGGER_PIN;
          this->statusPin = CHANNEL_4_STATUS_PIN;
          break;
      }

      this->armedPin = PYRO_ARM_PIN;
      this->channelNum = channelNum;

      this->fired = false;

      this->condition = condition;
    }

    void begin() {
      pinMode(this->triggerPin, OUTPUT);
      pinMode(this->statusPin, INPUT);

      pinMode(this->armedPin, INPUT);

      digitalWrite(this->triggerPin, LOW);
    }
    
    bool conntinuity() {
      // NEED PULL UP RESISTOR ON THE OUTPUT SIDE
      // Datasheet page 24 (10k ohm should be fine)

      return digitalRead(this->statusPin);
    }

    bool armed() {
      return digitalRead(this->armedPin);
    }

    void update(Data *data) {
      if (condition == nullptr) return;
      if (condition(data) && !fired)
      {
        digitalWrite(this->triggerPin, HIGH);

        fired = true;
      }

      switch (this->channelNum) {
        case 1:
          data->pyro_1_fired = fired;
          data->pyro_1_con = conntinuity();
          break;

        case 2:
          data->pyro_2_fired = fired;
          data->pyro_2_con = conntinuity();
          break;

        case 3:
          data->pyro_3_fired = fired;
          data->pyro_3_con = conntinuity();
          break;

        case 4:
          data->pyro_4_fired = fired;
          data->pyro_4_con = conntinuity();
          break;

        default: break;
      }
    }
};

#endif