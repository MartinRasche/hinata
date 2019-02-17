/****************************************************************\
*                                                                *
*    From Japanese 日向 (hinata) meaning "sunny place",          *
*    陽向 (hinata) meaning "toward the sun", or a non-           *
*    standard reading of 向日葵 (himawari) meaning "sunflower".  *
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

 
#include <Comms.h>

//#define _DEBUG_

#define SSID     "FONSIforEVER"
#define PASWD    "758-DRa47uK82ohneEnde" // well, i figured... i dont care

ESP8266* Comms::pt_wifi;

Comms::Comms(){
    _enabled = false;
}

void Comms::write(const char *msg, size_t length) {
	#ifdef _DEBUG_
	  Serial.write(msg, length);
	#endif
	uint8_t buffer[16] = {0};
	memset(buffer, ' ', sizeof(buffer));
	memcpy(&buffer, msg, length);	
	memory.setDispMsg(&buffer[0]);
}

void Comms::print(const char msg[]) {
	#ifdef _DEBUG_
	  Serial.print(msg);
	#endif	
	memory.setDispMsg(&msg[0]);
}

void Comms::print(const String &s) {
	#ifdef _DEBUG_
      Serial.write(s.c_str(), s.length());
	#endif
	uint8_t buffer[16] = {0};
	memset(buffer, ' ', sizeof(buffer));
	s.toCharArray(buffer, sizeof(buffer));
	memory.setDispMsg(&buffer[0]);
}

void Comms::enableWifi() {
  static ESP8266 wifi(Serial3, 115200);  
  
  print("FW Version: ");
  print(wifi.getVersion().c_str());    
  
  digitalWrite(LED_BUILTIN, HIGH);
  
	if (wifi.setOprToStation()) {
	  print("to station ok\r\n");

	  if(wifi.getLocalIP().length() > 12){
		memory.setCommsWebIP(wifi.getLocalIP());
		#ifdef _DEBUG_
		    Serial.println(wifi.getLocalIP().c_str()); 
		#endif
		if (wifi.enableMUX()) {
			print("multiple ok\r\n");		
			if (wifi.startTCPServer(80)) {
			  print("start server ok\r\n");
			  if (wifi.setTCPServerTimeout(10)) {
				digitalWrite(LED_BUILTIN, LOW);
				_enabled = true;
			  } else {
				print("set timout err\r\n");
			  }         
			} else {
			  print("start server err\r\n");
			}
		} else {
			print("multiple err\r\n");
	    } 
	  }else{
		if (wifi.joinAP(SSID, PASWD)) {
		  memory.setCommsWebIP(wifi.getLocalIP());
		  print("Join AP success\r\n");   
		  #ifdef _DEBUG_
		    Serial.println(wifi.getLocalIP().c_str()); 
		  #endif
		  if (wifi.enableMUX()) {
			print("multiple ok\r\n");
			if (wifi.startTCPServer(80)) {
			  print("start tcp server ok\r\n");
			  if (wifi.setTCPServerTimeout(10)) { 
				digitalWrite(LED_BUILTIN, LOW);
				_enabled = true;
			  } else {
				print("set tcp server timout err\r\n");
			  }         
			} else {
			  print("start tcp server err\r\n");
			}  
		  } else {
			print("multiple err\r\n");
		  }        
		} else {
		  print("Join AP err\r\n");
		}
	  }
	} else {
		print("to station err\r\n");       
	}
    
  pt_wifi = &wifi;
  
}

void Comms::getWifiMsg(){
  uint8_t buffer[128] = {0};
  uint8_t mux_id;    
  wifi_msg_length = pt_wifi->recv(&mux_id, buffer, sizeof(buffer), 100);  
  if (wifi_msg_length > 0) {
    digitalWrite(LED_BUILTIN, HIGH);       
	memory.setCommsWebMsg(&buffer[0]);		
  }else{
	memory.comms.COMMS_WEB_CYCLE_SINCE_MSG++;  
  }
}

void Comms::runWebInterface() {  
  uint8_t msgBuffer[128] = {0};  
  uint8_t cmdBuffer[4] = {0};
  uint8_t valBuffer[4] = {0};
  getWifiMsg();  
  if (wifi_msg_length > 0){
	  memcpy(msgBuffer, memory.getCommsWebMsg(), sizeof(msgBuffer));
	  #ifdef _DEBUG_
		  // Serial.println("msgbuffer: [");
		  // for(uint8_t i = 0; i < 16; i++) {
			// Serial.print((char)msgBuffer[i]);
		  // }
		  // Serial.print("]\r\n");
	  #endif
	  // find string in message 		  
	  if (strstr (msgBuffer, "/?") != -1){
		memcpy(cmdBuffer, msgBuffer + 6 /* Offset */, 3 /* Length */);
		memcpy(valBuffer, msgBuffer + 10 /* Offset */, 3 /* Length */); 
		
		cmdBuffer[3]='\0';
		valBuffer[3]='\0';
		String s;
		s = "cmd:";
		for (int i = 0; i < strlen(cmdBuffer); i++ ){
			s += (char)cmdBuffer[i];
		}
		s += " val:";
		for (int i = 0; i < strlen(valBuffer); i++ ){
			s += (char)valBuffer[i];
		}
		print(s);

		int value;
		sscanf(valBuffer, "%d", &value);   
		// save data to SharedMemory
		memory.setCommsWebCommand(&cmdBuffer[0]);
		memory.setCommsWebValue(&valBuffer[0]);
		// --------- set Axis
		if (strcmp(cmdBuffer, "sr1") == 0) {
		  memory.setAxisRotateAngle(value); 
		  digitalWrite(LED_BUILTIN, LOW);
		}
		if (strcmp(cmdBuffer, "sr2") == 0) {
		  memory.setAxisPitchAngle(value);
		  digitalWrite(LED_BUILTIN, LOW);
		}
		// --------- set Drive
		if (strcmp(cmdBuffer, "dir") == 0) {
			memory.comms.COMMS_WEB_CYCLE_SINCE_MSG = 0;
			if (strcmp(valBuffer, "fwd") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_FORWARD);
			if (strcmp(valBuffer, "bck") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_BACK);
			if (strcmp(valBuffer, "lft") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_LEFT);
			if (strcmp(valBuffer, "rgt") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_RIGHT);
			if (strcmp(valBuffer, "stp") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP);		 
			digitalWrite(LED_BUILTIN, LOW);
		}
		
	  }else if (strstr (msgBuffer, "GET") != -1) {
	   //generatWebPage(mux_id);
	  }	
  }
}
String UpTime()
{  
  long currentmillis=millis();
  long days=0;
  long hours=0;
  long mins=0;
  long secs=0;
  secs = currentmillis/1000; //convect milliseconds to seconds
  mins=secs/60; //convert seconds to minutes
  hours=mins/60; //convert minutes to hours
  days=hours/24; //convert hours to days
  secs=secs-(mins*60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins=mins-(hours*60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours=hours-(days*24); //subtract the coverted hours to days in order to display 23 hours max
  
  return String(days) + "d" + String(hours) + "h" + String(mins) + "m" + String(secs) + "s";
}
void Comms::generatWebPage(uint8_t mux_id){
    //float voltage1 = MeasureVoltage(VOLTAGE_PIN_1, 10000, 1000);
    //float voltage2 = MeasureVoltage(VOLTAGE_PIN_2, 10000, 1000); 
     
    // char charBuffer[500]= {0};
    
    // String strBuffer = "HTTP/1.1 200 OK\r\n"
                       // "Content-Type: text/html\r\n"
                       // "Connection: close\r\n"
                       // "Refresh: 8\r\n"
                       // "\r\n"
                       // "<!DOCTYPE HTML>\r\n"
                       // "<html>\r\n"
                       // "<head>\r\n"
                       // "<title>Voltage Display</title>\r\n"
                       // "<meta http-equiv=\"refresh\" content=\"5\">\r\n"
                       // "</head>\r\n"
                       // "<body>\r\n"
                       // "<h1>hinata Voltage Display</h1>\r\n"
                       // "<h2>Voltage wifi is: " + String(voltage1) + "</2>\r\n"
                       // "<h2>Voltage ardo is: " + String(voltage2) + "</2>\r\n"
                       // "<h2>UpTime is: " + UpTime() + "</2>\r\n"
                       // "</body>\r\n"
                       // "</html>\r\n";
                       
    // strBuffer.toCharArray(charBuffer, sizeof(charBuffer));
    // pt_wifi->send(mux_id, charBuffer, sizeof(charBuffer));

    // //gives time to send
    // delay(50);
}
