#include <Arduino.h>

char message[64];

void setup() {
    Serial.begin(9600);

    Serial.println("Ground Station Started");
}

void loop() {

    if (Serial.available()) {
        Serial.print(Serial.read());
    }

    /**
     * Packet order
     * 
     * Attention (255)
     * UUID
     * Packet ID
     * Data
     * CRC
     *
    */

}
