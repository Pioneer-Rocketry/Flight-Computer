// Memory.cpp
#include <LittleFS.h>
#include "Memory.h"

Memory::Memory(Data *data){
	this->data = data;

	flash = LittleFS_QSPIFlash();
}

bool Memory::begin() {
	Serial.println("Initializing SPI Flash...");

	if (!flash.begin()) {
		Serial.println("SPI Flash not detected.");
		return false;
	}

	flashFile = flash.open("datalog.txt", FILE_WRITE);
	if (!flashFile) {
		Serial.println("File not opened.");
		return false;
	}

	Serial.print("\nSpace Used = ");
	Serial.println(flash.usedSize());

	Serial.print("Filesystem Size = ");
	Serial.println(flash.totalSize());

	Serial.println("\nSPI Flash Initialized");

	return true;
}

void Memory::write_header() {
	// Log to flashFile
	if (!flashFile) {
		Serial.println("File not opened.");
		// while (1); 
		return;
	}

	flashFile.write(header.c_str(), header.length());
	flashFile.flush();
}

void Memory::write_data() {
	// Log to flashFile
	if (!flashFile) {
		Serial.println("File not opened.");
		// while (1); 
		return;
	}

	this->unloggedData += data->get_data() + "\n";

	if ((millis() - lastLog) >= interval) {
		start = millis();
		flashFile.write(unloggedData.c_str(), unloggedData.length());
		flashFile.flush();
		end = millis();

		Serial.print("Time to log: ");
		Serial.println(end - start);

		unloggedData = "";

		lastLog = millis();
	}
}

void Memory::erase_data(){
	Serial.println("\nErasing SPI Flash...");
	flash.lowLevelFormat('.');
	Serial.println("SPI Flash erased\n");

	flashFile = flash.open("datalog.txt", FILE_WRITE);
	if (!flashFile) {
		Serial.println("File not opened.");
		return;
	}
}

void Memory::dump_to_sd() {
	Serial.println("Dumping to SD card...");

	flashFile.flush();
	flashFile = flash.open("datalog.txt", FILE_READ);

	if (!sd.begin(SD_CONFIG)) {
		Serial.println("SD card initialization failed!");
		Serial.println("Make sure the SD card is formated for FatExt");
		sd.initErrorHalt(&Serial);
		while (1);
	}

	if (!sdFile.open("data.csv", O_RDWR | O_CREAT | O_TRUNC)) {
		Serial.println("open failed\n");
		return;
	}

	Serial.println("Copying data to SD card...");
	if (flashFile) {
		while (flashFile.available()) {
			sdFile.write(flashFile.read());
		}
		sdFile.sync();
		sdFile.close();
		
		flashFile.close();
	} else {
		Serial.println("File not found");
	}

	Serial.println("DATA DUMPED");
}

void Memory::print() {
	flashFile.flush();
	flashFile = flash.open("datalog.txt", FILE_READ);

	Serial.println("Printing flashFile");
	if (flashFile) {
		Serial.println("File Found");
		while (flashFile.available()) {
			Serial.write(flashFile.read());
		}
		flashFile.close();
	} else {
		Serial.println("File not found");
	}

	Serial.println("DATA DONE");
}


