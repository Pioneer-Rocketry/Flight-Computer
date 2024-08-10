// ColorLED.h
#ifndef COLOR_LED_hpp
#define COLOR_LED_hpp

#include <Arduino.h>

class ColorLED {
	private:
		static const int PIN_GREEN = 2;
		static const int PIN_RED   = 15;
		static const int PIN_BLUE  = 22;

		static void setColor(int red, int green, int blue) {
			digitalWrite(PIN_RED, red);
			digitalWrite(PIN_GREEN, green);
			digitalWrite(PIN_BLUE, blue);  
		}

	public:
		static void show_red() {
			setColor(1, 0, 0);  // red
		}

		static void show_green() {
			setColor(0, 1, 0);  // green
		}

		static void show_blue() {
			setColor(0, 0, 1);  // blue
		}

		static void show_yellow() {
			setColor(1, 1, 0);  // yellow
		}

		static void clear() {
			setColor(0, 0, 0);  // off
		} 
		
		static void begin() {
			pinMode(PIN_RED,    OUTPUT);
			pinMode(PIN_BLUE,   OUTPUT);
			pinMode(PIN_GREEN,  OUTPUT); 
		}

		static void flash_red() {
			show_red();
			delay(1000);
			clear();
		}

		ColorLED() { }
};

#endif