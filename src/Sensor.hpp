// Sensor.hpp
#ifndef SENSOR_h
#define SENSOR_h

#include <Arduino.h>
#include <Wire.h>

#include "Memory.h"
#include "Data.hpp"

class Sensor {
  
  private:
    TwoWire *wire;
    int address;

    virtual void update_sensor();


  protected:
    Data *data;

    void read(int registerAddress, byte* data, int bytes=1) {
      wire->beginTransmission(address);
      
      wire->write(registerAddress);
      wire->endTransmission(false);

      wire->requestFrom(address, bytes);
      for (int i=0; i<bytes; i++) {
        data[i] = wire->read();
      }

      wire->endTransmission(true);
    }

    void write(int registerAddress, byte data) {
      wire->beginTransmission(address);
      
      wire->write(registerAddress);
      wire->write(data);

      wire->endTransmission(true);
    }

    TwoWire* get_wire() {
      return wire;
    }

    int get_address() {
      return address;
    }

  public:
    virtual bool begin() = 0;
    virtual void get_data() = 0;

    Sensor(int address, Data *data, TwoWire *wire=&Wire) {
      this->address = address;
      this->data = data;
      this->wire = wire;
    }
};

#endif
