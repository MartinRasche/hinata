// Arduino lib for shared memory
// by Martin Rasche

#ifndef MEMORY_h
#define MEMORY_h

#include <Arduino.h>
#include <Memory.h>



#define TANKDRIVE_DIRECTION_STOP 0
#define TANKDRIVE_DIRECTION_FORWARD 1
#define TANKDRIVE_DIRECTION_BACK 2
#define TANKDRIVE_DIRECTION_LEFT 3
#define TANKDRIVE_DIRECTION_RIGHT 4


struct axis_m{
  int AXIS_ROTATE_ANGLE;
  int AXIS_PITCH_ANGLE; 
};
struct drive_m{
  uint8_t DIRECTION; 
};

struct comms_m{
  uint32_t COMMS_WEB_CYCLE_SINCE_MSG;
  String COMMS_WEB_MY_IP;
  uint8_t COMMS_WEB_MSG[128];
  uint8_t COMMS_WEB_COMMAND[4];
  uint8_t COMMS_WEB_VALUE[4];  
};
struct disp_m{
  uint8_t DISP_MSG[16]; 
};
struct sensor_m{
  float SENSOR_VOLTAGE; 
};

class Memory{	
	public:
	  Memory();
	  static axis_m axis;
	  static drive_m drive;
	  static comms_m comms;
	  static disp_m disp;
	  static sensor_m sensor;
	  
	  void setAxisRotateAngle(int angle);
	  void setAxisPitchAngle(int angle);
	  int getAxisRotateAngle();	  
	  int getAxisPitchAngle();	  
	  
	  void setDriveDirection(uint8_t dir);
	  uint8_t getDriveDirection();
	  
	  void setCommsWebIP(String msg);
	  void setCommsWebMsg(uint8_t* msg);
	  void setCommsWebCommand(uint8_t* msg);
	  void setCommsWebValue(uint8_t* value);	  
	  String getCommsWebIP();
	  uint8_t* getCommsWebMsg();
	  uint8_t* getCommsWebCommand();
	  uint8_t* getCommsWebValue();
	  
	  void setDispMsg(uint8_t* msg);
	  uint8_t* getDispMsg();
	  	  
	  void setSensorVoltage(float value);
	  float getSensorVoltage();
	  
};

#endif