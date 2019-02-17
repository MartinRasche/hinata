// Arduino lib for the HC-SR04 UltraSonic Sensor
// by Martin Rasche

#ifndef HC_SR04_h
#define HC_SR04_h

#include <Arduino.h>

class HC_SR04 {

  public:
    HC_SR04();
    int getDistance();
  private:
    int _ECHO;
    int _PULSE;
    
};
#endif