// Arduino lib for an OLED 
// by Martin Rasche

#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <Memory.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display{
  
  public:
    static Adafruit_SSD1306* pt_display;
    bool _enabled;  
    Display();		
	void enable(int OLED_RESET);
	void drawOLED();
		
  private:
    Memory memory;  
   
	
};

#endif