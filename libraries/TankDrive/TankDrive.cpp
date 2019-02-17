// Arduino lib for the L298N Dual H-Bridge Motor Driver
// by Martin Rasche

#include <TankDrive.h> 

TankDrive::TankDrive(){
	 _enabled = false;
}

void TankDrive::enable(int (*pins)[6]){
	 _motors.enable(pins);	 
	 _enabled = true;
}

void TankDrive::forward(){ 
    _motors.setLeftMotor(1, 1, MOTOR_SPEED);
	_motors.setRightMotor(1, 1, MOTOR_SPEED);
	memory.setDriveDirection(TANKDRIVE_DIRECTION_FORWARD);
  //Serial.println("Forward");
}

void TankDrive::back() {
    _motors.setLeftMotor(1, 0, MOTOR_SPEED);
	_motors.setRightMotor(1, 0, MOTOR_SPEED);
	memory.setDriveDirection(TANKDRIVE_DIRECTION_BACK);
  //Serial.println("Back");
}

void TankDrive::left() {
    _motors.setLeftMotor(1, 0, MOTOR_SPEED);
	_motors.setRightMotor(1, 1, MOTOR_SPEED);
	memory.setDriveDirection(TANKDRIVE_DIRECTION_LEFT);
  //Serial.println("Left");
}

void TankDrive::right() {
    _motors.setLeftMotor(1, 1, MOTOR_SPEED);
	_motors.setRightMotor(1, 0, MOTOR_SPEED);
	memory.setDriveDirection(TANKDRIVE_DIRECTION_RIGHT);
  //Serial.println("Right");
}

void TankDrive::stop() {
    _motors.setLeftMotor(0, 1, 0);
	_motors.setRightMotor(0 , 1, 0);
	memory.setDriveDirection(TANKDRIVE_DIRECTION_STOP);
  //Serial.println("Stop!");
} 

void TankDrive::setDirection(uint8_t direction){ 
    switch(direction){
		case TANKDRIVE_DIRECTION_STOP: stop(); break;
		case TANKDRIVE_DIRECTION_FORWARD: forward(); break;
		case TANKDRIVE_DIRECTION_BACK: back(); break;
		case TANKDRIVE_DIRECTION_LEFT: left(); break;
		case TANKDRIVE_DIRECTION_RIGHT: right(); break;
		default: stop(); break;
	}
  //Serial.println("Forward");
}

