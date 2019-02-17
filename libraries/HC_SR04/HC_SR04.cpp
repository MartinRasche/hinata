// Arduino lib for the HC-SR04 UltraSonic Sensor
// by Martin Rasche

#include <Arduino.h>
#include <HC_SR04.h> 

HC_SR04::HC_SR04() {
    _ECHO = A4;
    _PULSE = A5;

    pinMode(_ECHO, INPUT); // US echo   
    pinMode(_PULSE, OUTPUT);  // US pulse signal
}

//Ultrasonic distance measurement Sub function
int HC_SR04::getDistance() {
  digitalWrite(_PULSE, LOW);
  delayMicroseconds(2);
  digitalWrite(_PULSE, HIGH);
  delayMicroseconds(20);
  digitalWrite(_PULSE, LOW);   
  float Fdistance = pulseIn(_ECHO, HIGH);
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}