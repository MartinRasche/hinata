// Arduino lib for the L298N Dual H-Bridge Motor Driver
// by Martin Rasche


#include <L298N.h> 

L298N::L298N(){
	_enabled = false;
}

void L298N::enable(int (*pins)[6]){	
	_IN1 = (*pins)[0];
	_IN2 = (*pins)[1];
	_IN3 = (*pins)[2];
	_IN4 = (*pins)[3];
	_ENA = (*pins)[4];
	_ENB = (*pins)[5];  
	pinMode(_IN1, OUTPUT);
	pinMode(_IN2, OUTPUT);
	pinMode(_IN3, OUTPUT);
	pinMode(_IN4, OUTPUT);
	pinMode(_ENA, OUTPUT);
	pinMode(_ENB, OUTPUT);
	_enabled = true;
}

void L298N::setLeftMotor(bool enable, bool direction, int speed){ 
   if(_enabled){
	  if(enable){
		analogWrite(_ENB, speed);
		digitalWrite(_IN3, !direction);
		digitalWrite(_IN4, direction);
	  }else{
		 digitalWrite(_ENB, LOW); 
	  }	 
   }else{
	  Serial.println("L298N is disabled"); 
   }
}

void L298N::setRightMotor(bool enable, bool direction, int speed){
	if(_enabled){
	   if(enable){	
		analogWrite(_ENA, speed);
		digitalWrite(_IN1, !direction);
		digitalWrite(_IN2, direction);
	  }else{
		digitalWrite(_ENA, LOW); 
	  }
   }else{
	  Serial.println("L298N is disabled"); 
   } 
}


