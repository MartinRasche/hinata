// Arduino lib for the L298N Dual H-Bridge Motor Driver
// by Martin Rasche
//         _______
//        | L298N |
//        |       |
// V M1 --|1     4|-- M1 V
// G M1 --|2     3|-- M1 G
//        |_______|
//
//           \ /
//         forward
//

#ifndef L298N_h
#define L298N_h

#include <Arduino.h>

class L298N {
     
  public:
    L298N();
	void enable(int(*pins)[6]);
    void setLeftMotor(bool enable, bool direction, int speed);	
    void setRightMotor(bool enable, bool direction, int speed);
  private:
	bool _enabled;
	int _IN1;
	int _IN2;
	int _IN3;
	int _IN4;
	int _ENA;
	int _ENB;  
};
#endif