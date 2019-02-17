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
#ifndef FC_51_h
#define FC_51_h

#include <Arduino.h>

class FC_51{

	public:
		FC_51();
		void enable(int pin);
		bool isObstructed();

	private:
		bool _enabled;
		int _pin;
};

#endif