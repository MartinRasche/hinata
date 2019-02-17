// Arduino lib for the Servos which controle the 
// Solar-Panel-Plattform 
// by Martin Rasche

#include <AxisArm.h> 

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

