// Arduino lib for the Servos which controle the 
// Solar-Panel-Plattform
// by Martin Rasche

#ifndef AXISARM_h
#define AXISARM_h

#include <Arduino.h>
#include <VarSpeedServo.h>   //servo library

class AxisArm{
  
  public:   
    bool _enabled;	
    AxisArm();
		
	void enable(int pin1, int pin2);
	int getAxis(int axis);
	void setAxis(int axis, int degree);
	void setAxis(int axis, int degree, int speed);
	
  private:
    int _pin;
	  VarSpeedServo _servo[2];
    
	
};
#endif