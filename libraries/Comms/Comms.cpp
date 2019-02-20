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
  uint8_t buffer[256] = {0};
  uint8_t mux_id;    
  wifi_msg_length = pt_wifi->recv(&mux_id, buffer, buffer, 100);  
  if (wifi_msg_length > 0) {    
	memory.setCommsWebMsg(&buffer[0]);		
  }else{
	memory.comms.last_updated++;  
  }
}

void Comms::runWebInterface() {  
  uint8_t msgBuffer[256] = {0};  
  uint8_t cmdBuffer[4] = {0};
  uint8_t lgtBuffer[4] = {0};
  uint8_t valBuffer[248] = {0};
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
	  if (strstr (msgBuffer, "/?") != -1){
		memcpy(cmdBuffer, msgBuffer + 6 /* Offset */, 3 /* Length */);
		memcpy(lgtBuffer, msgBuffer + 10 /* Offset */, 3 /* Length */);
		cmdBuffer[3]='\0';
		lgtBuffer[3]='\0';
		int msg_length;
		sscanf(lgtBuffer, "%d", &msg_length); 
		memcpy(valBuffer, msgBuffer + 14 /* Offset */, msg_length /* Length */); 		
		valBuffer[msg_length]='\0';
		
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
		{
			// Split the command in two values
			char* separator = strchr(command, ':');
			if (separator != 0)
			{
				// Actually split the string in 2: replace ':' with 0
				*separator = 0;
				//int Id = atoi(command);
				
				if (strcmp(command, "s1h") == 0){++separator;memory.gamepad.s1h = atof(separator);}
				if (strcmp(command, "s1v") == 0){++separator;memory.gamepad.s1v = atof(separator);}	
				if (strcmp(command, "s2h") == 0){++separator;memory.gamepad.s2h = atof(separator);}
				if (strcmp(command, "s2v") == 0){++separator;memory.gamepad.s2v = atof(separator);}	
				if (strcmp(command, "b01") == 0){++separator;memory.gamepad.b01 = atoi(separator);}
				if (strcmp(command, "b02") == 0){++separator;memory.gamepad.b02 = atoi(separator);}	
				if (strcmp(command, "b03") == 0){++separator;memory.gamepad.b03 = atoi(separator);}
				if (strcmp(command, "b04") == 0){++separator;memory.gamepad.b04 = atoi(separator);}
				if (strcmp(command, "b05") == 0){++separator;memory.gamepad.b05 = atoi(separator);}
				if (strcmp(command, "b06") == 0){++separator;memory.gamepad.b06 = atoi(separator);}	
				if (strcmp(command, "b07") == 0){++separator;memory.gamepad.b07 = atoi(separator);}
				if (strcmp(command, "b08") == 0){++separator;memory.gamepad.b08 = atoi(separator);}	
				if (strcmp(command, "b09") == 0){++separator;memory.gamepad.b09 = atoi(separator);}
				if (strcmp(command, "b10") == 0){++separator;memory.gamepad.b10 = atoi(separator);}	
				if (strcmp(command, "b11") == 0){++separator;memory.gamepad.b11 = atoi(separator);}
				if (strcmp(command, "b12") == 0){++separator;memory.gamepad.b12 = atoi(separator);}
			    if (strcmp(command, "b13") == 0){++separator;memory.gamepad.b13 = atoi(separator);}	
				if (strcmp(command, "b14") == 0){++separator;memory.gamepad.b14 = atoi(separator);}
				if (strcmp(command, "b15") == 0){++separator;memory.gamepad.b15 = atoi(separator);}	
				if (strcmp(command, "b16") == 0){++separator;memory.gamepad.b16 = atoi(separator);}
				if (strcmp(command, "b17") == 0){++separator;memory.gamepad.b17 = atoi(separator);}

			}
			// Find the next command in input string
			command = strtok(0, ";");
		}
		Serial.println(command);
						
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
	/* 	if (memory.gamepad.s1h < -0.85){ 
		  memory.setDriveDirection(TANKDRIVE_DIRECTION_LEFT);
		}else if (memory.gamepad.s1h > 0.85){ 
		  memory.setDriveDirection(TANKDRIVE_DIRECTION_RIGHT);
		}else{
		  memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP);
		}
		
		if (memory.gamepad.s1v < -0.85){
		  memory.setDriveDirection(TANKDRIVE_DIRECTION_FORWARD);		  
		}else if (memory.gamepad.s1v > 0.85){ 
		  memory.setDriveDirection(TANKDRIVE_DIRECTION_BACK);
		}else{
		  memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP);				
		} */
		
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
		
		Serial.println("\n");
		for (int i = 0; i < sizeof(directions); i++ ){
			Serial.println(directions[i]);
		}
		Serial.println("\n");
		
/* 		String s;
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
		}
		if (strcmp(cmdBuffer, "sr2") == 0) {
		  memory.setAxisPitchAngle(value);
		}
		// ---- set Axis - right stick
		if (strcmp(cmdBuffer, "s2v") == 0) {
			memory.comms.COMMS_WEB_CYCLE_SINCE_MSG = 0;			
			if (value > 0.85){ 
			  memory.axis.AXIS_ROTATE = true;
			  memory.axis.AXIS_ROTATE_ANGLE = 0;
			}
			else if (value < -0.85){ 
			  memory.axis.AXIS_ROTATE = true;
			  memory.axis.AXIS_ROTATE_ANGLE = 180;
			}else{
			  memory.axis.AXIS_ROTATE = false;
			}
			
		}else if (strcmp(cmdBuffer, "s2h") == 0){
			memory.comms.COMMS_WEB_CYCLE_SINCE_MSG = 0;
			if (value > 0.85){
			  memory.axis.AXIS_PITCH = true;					
			  memory.axis.AXIS_PITCH_ANGLE = 180;
			  
			}else if (value < -0.85){ 
			  memory.axis.AXIS_PITCH = true;
			  memory.axis.AXIS_PITCH_ANGLE = 0;
			}else{
			  memory.axis.AXIS_PITCH = false;				
			}
			
		} */
		
		// --------- set Drive direction
		// if (strcmp(cmdBuffer, "dir") == 0) {
			// memory.comms.COMMS_WEB_CYCLE_SINCE_MSG = 0;
			// if (strcmp(valBuffer, "fwd") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_FORWARD);
			// if (strcmp(valBuffer, "bck") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_BACK);
			// if (strcmp(valBuffer, "lft") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_LEFT);
			// if (strcmp(valBuffer, "rgt") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_RIGHT);
			// if (strcmp(valBuffer, "stp") == 0) memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP);		 
			// digitalWrite(LED_BUILTIN, LOW);
		// }
		
		// ---- set Drive - left stick
	/* 	if (strcmp(cmdBuffer, "s1h") == 0) {
			memory.comms.COMMS_WEB_CYCLE_SINCE_MSG = 0;
			if (value > 0.85){ 
			  memory.setDriveDirection(TANKDRIVE_DIRECTION_FORWARD);
			}else if (value < -0.85){ 
			  memory.setDriveDirection(TANKDRIVE_DIRECTION_BACK);
			}else{
		      memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP);
			}
		}else if (strcmp(cmdBuffer, "s1v") == 0){
			memory.comms.COMMS_WEB_CYCLE_SINCE_MSG = 0;
			if (value > 0.85){ 
			  memory.setDriveDirection(TANKDRIVE_DIRECTION_LEFT);
			}else if (value < -0.85){ 
			  memory.setDriveDirection(TANKDRIVE_DIRECTION_RIGHT);
			}else{
			  memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP);
			}
		} */
		
		
		
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
