// Arduino lib for the KY-022 Infrared-Receiver 
// by Martin Rasche

#ifndef KY_H022_h
#define KY_H022_h

#include <Arduino.h>
#include <IRremote.h>

////////// IR REMOTE CODES //////////
#define IR_F 16736925  // FORWARD
#define IR_B 16754775  // BACK
#define IR_L 16720605  // LEFT
#define IR_R 16761405  // RIGHT
#define IR_S 16712445  // STOP
#define NP1 16738455  // button 1
#define NP2 16750695  // button 2
#define NP3 16756815  // button 3
#define NP4 16724175  // button 4
#define NP5 16718055  // button 5
#define NP6 16743045  // button 6
#define NP7 16716015  // button 7
#define NP8 16726215  // button 8
#define NP9 16734885  // button 9
#define NP0 16730805  // button 0
#define SP1 16728765  // button star
#define SP2 16732845  // button hash
#define UNKNOWN_IR_F 5316027     // FORWARD
#define UNKNOWN_IR_B 2747854299  // BACK
#define UNKNOWN_IR_L 1386468383  // LEFT
#define UNKNOWN_IR_R 553536955   // RIGHT
#define UNKNOWN_IR_S 3622325019  // STOP
#define UNKNOWN_NP1 1386468383 // button 1


class KY_022{
  
  public: 
    KY_022();
	void init(int pin);
    unsigned long IRvalue; 	
    decode_results results;   //Define structure type	        
    bool getKey(unsigned long val);
    
  private:
    IRrecv* _p_irrecv;
};

#endif