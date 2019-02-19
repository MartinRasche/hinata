// Arduino lib for the Servos which controle the 
// Solar-Panel-Plattform 
// by Martin Rasche

#include <AxisArm.h> 

VarSpeedServo AxisArm::_servo[2];

AxisArm::AxisArm(){
    this->_enabled = false;
}

void AxisArm::enable(int pin1, int pin2){
	_servo[0].attach(pin1);
    _servo[1].attach(pin2);
    this->_enabled = true;
}

int AxisArm::getAxis(int axis){
    if( 0 <= axis <= 1){
	      return _servo[axis].read();  
    }
    return -1;
}

void AxisArm::setAxis(int axis, int degree){
    if (0 <= axis <= 1) {
        _servo[axis].write(degree);
    }
}

void AxisArm::setAxis(int axis, int degree, int speed){
    if (0 <= axis <= 1) {
        _servo[axis].write(degree, speed);
    }
}

void AxisArm::stop(int axis){	
	if (0 <= axis <= 1) {
		if(axis == 0){
			memory.axis.AXIS_ROTATE = false;
			memory.axis.AXIS_ROTATE_ANGLE = getAxis(0);
		}
		if(axis == 1){
			memory.axis.AXIS_PITCH = false;
			memory.axis.AXIS_PITCH_ANGLE = getAxis(1);
		}
        
    }
}

