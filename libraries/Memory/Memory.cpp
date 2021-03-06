// Arduino lib for shared memory
// by Martin Rasche

#include <Memory.h> 

axis_m Memory::axis;
drive_m Memory::drive;
comms_m Memory::comms;
comms_gamepad_m Memory::gamepad;
disp_m Memory::disp;
sensor_m Memory::sensor;

Memory::Memory(){	
    axis.AXIS_ROTATE = false;
	axis.AXIS_ROTATE_ANGLE = 90;
	axis.AXIS_PITCH = false;
	axis.AXIS_PITCH_ANGLE = 90;
	drive.DIRECTION = TANKDRIVE_DIRECTION_STOP;
	drive.LEFT_TICKS = 0;
	drive.RIGHT_TICKS = 0;
	drive.MOTOR_SPEED_L = 150;   
    drive.MOTOR_SPEED_R = 120;
	comms.last_updated = 0;
	gamepad.last_updated = 0;
	gamepad.s1h = 0;
	gamepad.s1v = 0;
	gamepad.s2h = 0;
	gamepad.s2v = 0;
	gamepad.btn = 0;
	memset(comms.COMMS_WEB_MSG, 0, sizeof(comms.COMMS_WEB_MSG));
	memset(comms.COMMS_WEB_COMMAND, 0, sizeof(comms.COMMS_WEB_COMMAND));
	memset(comms.COMMS_WEB_VALUE, 0, sizeof(comms.COMMS_WEB_VALUE));
	memset(disp.DISP_MSG, 0, sizeof(disp.DISP_MSG));		
}

void Memory::setAxisRotateAngle(int angle){
	axis.AXIS_ROTATE_ANGLE = angle;	
}
void Memory::setAxisPitchAngle(int angle){
	axis.AXIS_PITCH_ANGLE = angle;	
}
int Memory::getAxisRotateAngle(){
	return axis.AXIS_ROTATE_ANGLE;	
}
int Memory::getAxisPitchAngle(){
	return axis.AXIS_PITCH_ANGLE;	
}


void Memory::setDriveDirection(uint8_t dir){
	drive.DIRECTION = dir;
}
uint8_t Memory::getDriveDirection(){	
	return drive.DIRECTION;	
}


void Memory::setCommsWebIP(String msg){
	comms.COMMS_WEB_MY_IP = msg;	
}
void Memory::setCommsWebMsg(uint8_t* msg){
	memcpy(&comms.COMMS_WEB_MSG, msg, sizeof(comms.COMMS_WEB_MSG));
}
void Memory::setCommsWebCommand(uint8_t* msg){
	memcpy(&comms.COMMS_WEB_COMMAND, msg, sizeof(comms.COMMS_WEB_COMMAND));	
}
void Memory::setCommsWebValue(uint8_t* value){
	memcpy(&comms.COMMS_WEB_VALUE, value, sizeof(comms.COMMS_WEB_VALUE));	
}
String Memory::getCommsWebIP(){
	return comms.COMMS_WEB_MY_IP;	
}
uint8_t* Memory::getCommsWebMsg(){	
	return comms.COMMS_WEB_MSG;	
}
uint8_t* Memory::getCommsWebCommand(){
	return comms.COMMS_WEB_COMMAND;	
}
uint8_t* Memory::getCommsWebValue(){
	return comms.COMMS_WEB_VALUE;	
}



void Memory::setDispMsg(uint8_t* msg){
	memcpy(&disp.DISP_MSG, msg, sizeof(disp.DISP_MSG));
}
uint8_t* Memory::getDispMsg(){	
	return disp.DISP_MSG;	
}



void Memory::setSensorVoltage(float value){
	sensor.SENSOR_VOLTAGE = value;
}
float Memory::getSensorVoltage(){	
	return sensor.SENSOR_VOLTAGE;	
}
