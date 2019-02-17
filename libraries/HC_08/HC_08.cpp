// Arduino lib for the HC-08 Bluetooth Receiver/Transmitter
// by Martin Rasche

#include <Arduino.h>
#include <HC_08.h> 

HC_08::HC_08(){

}


char HC_08::read(){
	return Serial.read();  // attach servo on pin 3 to servo object     
}

void HC_08::write(String msg){
    for(int i=0; i < msg.length(); i++)
        Serial.write(msg[i]);  // attach servo on pin 3 to servo object
}
