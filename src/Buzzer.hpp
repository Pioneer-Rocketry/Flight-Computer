// Buzzer.h
#ifndef BUZZER_hpp
#define BUZZER_hpp

#include <Arduino.h>

class Buzzer {

  private:
    static const int PIN_BUZZER = 17;

  public:
    static void end_sound() {
      tone(PIN_BUZZER, 400);
      delay(1000);
      noTone(PIN_BUZZER);
    }

    static void start_sound() {
      tone(PIN_BUZZER, 1000);
      delay(200);
      noTone(PIN_BUZZER);
    }

    static void start_up_sound() {
      for (int i=100; i<=1000; i+=100){
        tone(PIN_BUZZER, i);
        delay(100);
      }
      noTone(PIN_BUZZER);
    }


    static void begin() {
      pinMode(PIN_BUZZER, OUTPUT);
    }
};
#endif