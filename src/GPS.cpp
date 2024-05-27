#include "GPS.h"

GPS::GPS(HardwareSerial* serial, int baud) : Sensor(0x00) { // No address for GPS cause it's serial
  this->serial = serial;
  this->baud = baud;
}

bool GPS::begin() {
  this->serial->begin(baud);

  // Set the settings we want to use
  // - 10hz update rate
  this->serial->println("$PMTK220,100*2F"); // 10hz update rate

  return true;
}

void GPS::update_sensor() {
  // Read the Serial String and process it
  // THIS COULD BE A BOTTLNECK
  while (serial->available() > 0) {
    gps.encode(serial->read());
  }
}

void GPS::get_data(Data *data) {
  update_sensor();

  data->gps_lat   = gps.location.lat();
  data->gps_lng   = gps.location.lng();
  data->gps_sat   = gps.satellites.value();
  data->gps_alt   = gps.altitude.meters();
  data->gps_speed = gps.speed.kmph();
  data->gps_hdop  = gps.hdop.hdop();
  
  // Read the GPS data
  // Decode the NMEA Data (https://docs.arduino.cc/learn/communication/gps-nmea-data-101)
  // Most likely the module we are going to fly (https://cdn-shop.adafruit.com/product-files/746/CD+PA1616S+Datasheet.v03.pdf)
}

