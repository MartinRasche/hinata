// Arduino lib for the KY-022 Infrared-Receiver 
// by Martin Rasche

#include <Arduino.h>
#include <KY_022.h> 

KY_022::KY_022(){
    	
    IRvalue = 0;
}

void KY_022::init(int pin){
	IRrecv irrecv(pin);    //initialization
    _p_irrecv = &irrecv;
    _p_irrecv->enableIRIn();  // Start receiving
}

bool KY_022::getKey(unsigned long val){
    if (_p_irrecv->decode(&results)) { 
        IRvalue = results.value;
        Serial.println(IRvalue);
        _p_irrecv->resume();      // Receive the next value
       
        if(IRvalue == val) {  
            return true;
        }	
    }
    return false;
}

