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


#ifndef COMMS_h
#define COMMS_h

#include <Arduino.h>
#include <Memory.h>

#include <ESP8266.h> // http://docs.iteadstudio.com/ITEADLIB_Arduino_WeeESP8266/examples.html



class Comms {
  public:
	static ESP8266* pt_wifi;
    bool _enabled;
    Comms();
	void enableWifi();
	void print(const char msg[]);
	void print(const String &s);
	void write(const char *msg, size_t length);
    void getWifiMsg();
	void runWebInterface();
	void generatWebPage(uint8_t mux_id);
			
	private:
	  Memory memory;
	  uint32_t wifi_msg_length;
	  	       
};

#endif