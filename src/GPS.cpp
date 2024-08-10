#include "GPS.h"

GPS::GPS(Data *data, HardwareSerial* serial, int baud) : Sensor(0x00, data) { // No address for GPS cause it's serial
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

void GPS::get_data() {
	update_sensor();

	lastTime = data->gps.time;

	if (data->starting_gps_alt == -999 && gps.altitude.isUpdated()) {
		data->starting_gps_alt = gps.altitude.meters();
	}

	data->gps.time  = gps.time.second() + gps.time.minute()*60;
	data->gps.lat   = gps.location.lat();
	data->gps.lng   = gps.location.lng();
	data->gps.sat   = gps.satellites.value();
	data->gps.alt   = gps.altitude.meters();
	data->gps.speed = gps.speed.kmph();
	data->gps.hdop  = gps.hdop.hdop();

	data->newGPSdata = lastTime != data->gps.time;
	
	// Read the GPS data
	// Decode the NMEA Data (https://docs.arduino.cc/learn/communication/gps-nmea-data-101)
	// Most likely the module we are going to fly (https://cdn-shop.adafruit.com/product-files/746/CD+PA1616S+Datasheet.v03.pdf)
}

