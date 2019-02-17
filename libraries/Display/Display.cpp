// Arduino lib for an OLED 
// by Martin Rasche

#include <Display.h> 

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16



//Adafruit_SSD1306 display(OLED_RESET);

Adafruit_SSD1306* Display::pt_display;

Display::Display(){
    this->_enabled = false;	
}


void Display::enable(int OLED_RESET){
  static const unsigned char PROGMEM logo16_glcd_bmp[] =
			{ B00000000, B11000000,
			  B00000001, B11000000,
			  B00000001, B11000000,
			  B00000011, B11100000,
			  B11110011, B11100000,
			  B11111110, B11111000,
			  B01111110, B11111111,
			  B00110011, B10011111,
			  B00011111, B11111100,
			  B00001101, B01110000,
			  B00011011, B10100000,
			  B00111111, B11100000,
			  B00111111, B11110000,
			  B01111100, B11110000,
			  B01110000, B01110000,
			  B00000000, B00110000
			};	
  static Adafruit_SSD1306 disp(OLED_RESET);  
  // initialize and clear display
  disp.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  disp.clearDisplay();
  disp.display(); 
  disp.setRotation(2);
  // display a line of text
  disp.setTextSize(1);
  disp.setTextColor(WHITE);
  disp.setCursor(0, 0);
  disp.clearDisplay();
  disp.println("OLED SETUP DONE");
  disp.display();  
  
  pt_display = &disp;
  this->_enabled = true;  
}

void Display::drawOLED(){  
  uint8_t buffer[16] = {0};
 
  pt_display->setTextSize(1);
  pt_display->setTextColor(WHITE);
  pt_display->setCursor(0, 0);
  pt_display->clearDisplay();
  
  // OLED 1st Line
  memcpy(&buffer[1], memory.getCommsWebCommand(), 4);
  memcpy(&buffer[5], memory.getCommsWebValue(), 4);
  for(uint8_t i = 0; i < 10; i++) {
    pt_display->print((char)buffer[i]);
	//Serial.print((char)buffer[i]);
  }
  pt_display->setCursor(60, 0);
  pt_display->print(memory.getSensorVoltage());
  pt_display->print("V");
  
  
  // OLED 2nd Line
  pt_display->setCursor(0, 8);
  pt_display->print(memory.getCommsWebIP());
  
  
  // OLED 3rd Line
  pt_display->setCursor(0, 16);
  memset(buffer, 0, sizeof(buffer));
  memcpy(buffer, memory.getDispMsg(), sizeof(buffer));
  for(uint8_t i = 0; i < 16; i++) {
    pt_display->print((char)buffer[i]);
	//Serial.print((char)buffer[i]);
  }
    
  pt_display->display();
  
}

