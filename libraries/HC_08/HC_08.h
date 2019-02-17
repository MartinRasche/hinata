// Arduino lib for the HC-08 Bluetooth Receiver/Transmitter
// by Martin Rasche

#ifndef HC_08_h
#define HC_08_h

#include <Arduino.h>

class HC_08 {
	
  public:  
    HC_08();
	  char read();
	  void write(String msg);
  
};
#endif