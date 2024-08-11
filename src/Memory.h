// Memory.h
#ifndef MEMORY_h
#define MEMORY_h

#include <LittleFS.h>
#include <Arduino.h>
#include <SPI.h>

#include <RingBuf.h>
#include <SdFat.h>

#include "Data.hpp"

#define SD_CS_PIN SDCARD_SS_PIN
#define SD_CONFIG SdioConfig(FIFO_SDIO)

class Memory {
	private:
		// Flash
		LittleFS_QSPIFlash flash;
		File flashFile;

		// SD
		SdExFat sd;
		ExFile sdFile;

		unsigned long start = 0;
		unsigned long end = 0;

		float log_freq = .1; // logging frequency in Hz (every 10 seconds)
		unsigned long interval = 10000; // override to every 10 sec // = 1000 / log_freq; // logging interval in ms
		unsigned long lastLog = 0;

		String dataStr = "";
		String unloggedData = "";

		Data *data;

	public:
		const String header = ""; // = "Time,AccX,AccY,AccZ,GyrX,GyrY,GyrZ,MagX,MagY,MagZ,Heading,AngleX,AngleY,AngleZ,VelX,VelY,VelZ,Temp,Press,Alti,F_Alt,State,GPS_LAT,GPS_LNG,GPS_SAT,GPS_ALT,GPS_SPEED,GPS_HDOP";
		
		void write_header();
		void write_data();
		void erase_data();
		void dump_to_sd();
		void print();

		bool begin();

		Memory(Data *data);
};

#endif