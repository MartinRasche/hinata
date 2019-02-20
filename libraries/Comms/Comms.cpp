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
  wifi_msg_length = pt_wifi->recv(&mux_id, buffer, sizeof(buffer), 60);  
  if (wifi_msg_length > 0) {    
	memory.setCommsWebMsg(&buffer[0]);		
  }else{
	memory.comms.last_updated++;  
  }
}


int32_t hash(uint8_t *str)
{
    int32_t hash = 0;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void Comms::runWebInterface() {  
  uint8_t msgBuffer[128] = {0};  
  uint8_t valBuffer[64] = {0};
  getWifiMsg();  
  if (wifi_msg_length > 0){
	  memcpy(msgBuffer, memory.getCommsWebMsg(), sizeof(msgBuffer));
	  #ifdef _DEBUG_
		  Serial.println("msgbuffer: [");
		  for(uint8_t i = 0; i < sizeof(msgBuffer); i++) {
			Serial.print((char)msgBuffer[i]);
		  }
		  Serial.print("]\r\n");
	  #endif
	  // find string in message 
      // example>	  
	  // +IPD,2,287:GET /?36;s1h:0;s1v:0;s2h:0.03;s2v:0;btn:0000; HTTP/1.1	  
	  if (strstr (msgBuffer, "/?") != -1){ 
		char* msgBufferRest = msgBuffer; 	  
	    char* pt_val_length = strtok_r(msgBuffer, "?", &msgBufferRest);
		pt_val_length = strtok_r(msgBufferRest,";", &msgBufferRest);				
		if(isDigit(*pt_val_length)){			
			char* pt_checksum = strtok_r(msgBufferRest,";", &msgBufferRest);			
			int val_length;
			sscanf(pt_val_length, "%d", &val_length);
			memcpy(valBuffer, msgBufferRest /* Offset */, val_length /* Length */); 
			valBuffer[val_length]='\0';
			
			//check checksum
			char str[11];
			sprintf_P(str, (PGM_P)F("%ld"), hash(valBuffer));			
			if (strcmp((char*)pt_checksum, str) == 0){
											
				#ifdef _DEBUG_
					Serial.println("valBuffer: [");
					  for(uint8_t i = 0; i < sizeof(valBuffer); i++) {
						Serial.print((char)valBuffer[i]);
					  }
					  Serial.print("]\r\n");		
					Serial.println();
				#endif		
				// Read each command pair 
				char* command = strtok(valBuffer, ";");
				memory.comms.last_updated = 0;
				while (command != 0)
				{	// Split the command in two values
					char* separator = strchr(command, ':');
					if (separator != 0)
					{	// Actually split the string in 2: replace ':' with 0
						*separator = 0;					
						if (strcmp(command, "s1h") == 0){++separator;memory.gamepad.s1h = atof(separator);}
						if (strcmp(command, "s1v") == 0){++separator;memory.gamepad.s1v = atof(separator);}	
						if (strcmp(command, "s2h") == 0){++separator;memory.gamepad.s2h = atof(separator);}
						if (strcmp(command, "s2v") == 0){++separator;memory.gamepad.s2v = atof(separator);}	
						if (strcmp(command, "btn") == 0){++separator;memory.gamepad.btn = strtol(separator, 0, 16);}
					}
					// Find the next command in input string
					command = strtok(0, ";");
				}
			}				
		}
								
		// ---- set Axis - right stick
		if (memory.gamepad.s2h > 0.85){
		  memory.axis.AXIS_ROTATE = true;					
		  memory.axis.AXIS_ROTATE_ANGLE = 0;
		  
		}else if (memory.gamepad.s2h < -0.85){ 
		  memory.axis.AXIS_ROTATE = true;
		  memory.axis.AXIS_ROTATE_ANGLE = 180;
		}else{
		  memory.axis.AXIS_ROTATE = false;				
		}
		
		if (memory.gamepad.s2v > 0.85){ 
		  memory.axis.AXIS_PITCH = true;
		  memory.axis.AXIS_PITCH_ANGLE = 180;
		}
		else if (memory.gamepad.s2v < -0.85){ 
		  memory.axis.AXIS_PITCH = true;
		  memory.axis.AXIS_PITCH_ANGLE = 0;
		}else{
		  memory.axis.AXIS_PITCH = false;
		}
		
		// ---- set Drive - left stick		
			
		float directions[4];
		directions[0] = abs(-1-memory.gamepad.s1v);  // forward
		directions[1] = abs(1-memory.gamepad.s1v);  // back
		directions[2] = abs(-1-memory.gamepad.s1h);  // left
		directions[3] = abs(1-memory.gamepad.s1h);  // right
		// get closest index to target for each direction		
		uint8_t index = 4;
		float targetVal = 0.15;  // we search for the closest value -> 1 - 0.85 = 0.15
		for(uint8_t i = 0; i < 4; i++)
		{
			if(directions[i] < targetVal) {
				targetVal = directions[i];
				index = i;
			}
		}
		switch(index){
			case 0: memory.setDriveDirection(TANKDRIVE_DIRECTION_FORWARD); break;
			case 1: memory.setDriveDirection(TANKDRIVE_DIRECTION_BACK); break;
			case 2: memory.setDriveDirection(TANKDRIVE_DIRECTION_LEFT); break;
			case 3: memory.setDriveDirection(TANKDRIVE_DIRECTION_RIGHT); break;
			case 4: memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP); break;
			default: memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP); break;
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
