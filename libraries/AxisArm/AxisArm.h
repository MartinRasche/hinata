// Arduino lib for the Servos which controle the 
// Solar-Panel-Plattform
// by Martin Rasche

#ifndef AXISARM_h
#define AXISARM_h

#include <Arduino.h>
#include <Memory.h>
#include <VarSpeedServo.h>   //servo library

class AxisArm{
  
  public:
    static VarSpeedServo _servo[2];
    bool _enabled;	
    AxisArm();
		
	void enable(int pin1, int pin2);
	int getAxis(int axis);
	void setAxis(int axis, int degree);
	void setAxis(int axis, int degree, int speed);
	void stop(int axis);
	
  private:
    Memory memory;
    int _pin;
	  
    
	
};
#endif