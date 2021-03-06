/****************************************************************\
*                                                                *
*    From Japanese 日向 (hinata) meaning "sunny place",           *
*    陽向 (hinata) meaning "toward the sun", or a non-            *
*    standard reading of 向日葵 (himawari) meaning "sunflower".    *
*                                                                *
******************************************************************
*                                                                *
*    Tank-Platform with                                          *
*       Solar-Panels on 2 Servos                                 *
*       Photodiodes for Solar-Panel Movement                     *
*       IR-Distance-Sensors for not running into Stuff           *
*       WirelessLan-Adapter for Communication                    *
*       H-Brigde for the 2 Traks                                 *
*    by Martin Rasche                                            *
*                                                                *
\****************************************************************/
/****************************************************************\
*   FC-51 IR-Distance-Sensors Library                            *
*                                                                *
*                        /E\./R\    E - IR Emitter               *
*                       |   |   |   R - IR Receiver              *
*                       |      _|                                *
*                       |HHH  |X|                                *
*                       |-     -|                                *
*                       |o  O  o|                                *
*                       |_______|   D - Data output              *
*                        |  |  |    V - Voltage input            *
*                        D  V  G    G - Ground                   *
*                                                                *
\****************************************************************/
 
#include <FC_51.h>


FC_51::FC_51(){
	this->_enabled = false;
}

void FC_51::enable(int pin){
  _pin = pin;
	pinMode(_pin, INPUT);
	this->_enabled = true;
}

bool FC_51::isObstructed(){
	
	if(_enabled){
		if(!digitalRead(_pin)){
			return true;     
		}		
	}
	
	return false;
}