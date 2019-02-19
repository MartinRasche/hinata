// Arduino lib for the L298N Dual H-Bridge Motor Driver
// by Martin Rasche
//         _______
//        | L298N |
//        |       |
//   M ---|1     4|--- M
//   1 ---|2     3|--- 2
//        |_______|
//
//           \ /
//         forward
//

#ifndef TANKDRIVE_h
#define TANKDRIVE_h

#include <Arduino.h>
#include <Memory.h>
#include <L298N.h> 



class TankDrive {
     
  public:
    bool _enabled;
    TankDrive();
	static L298N _motors;
	void enable(int (*pins)[7]);
    void forward();	
    void back();
    void left();
    void right();
	void stop();
	void disable();
	void setDirection(uint8_t direction);	
	
  private:
	Memory memory;
};
#endif