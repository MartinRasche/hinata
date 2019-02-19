

/****************************************************************\
*                                                                *
     From Japanese 日向 (hinata) meaning "sunny place",
     陽向 (hinata) meaning "toward the sun", or a non-
     standard reading of 向日葵 (himawari) meaning "sunflower".
*                                                                *
******************************************************************
*                                                                *
     Tank-Platform with
        Solar-Panels on 2 Servos
        Photodiodes for Solar-Panel Movement
        IR-Distance-Sensors for not running into Stuff
        WirelessLan-Adapter for Communication
        H-Brigde for the 2 Traks
     by Martin Rasche
*                                                                *
\****************************************************************/
#include <math.h>

#include <AxisArm.h>
#include <Comms.h>
#include <Display.h>
#include <Memory.h>
#include <TankDrive.h>
#include <TaskScheduler.h>




// https://www.arduino.cc/en/Hacking/PinMapping2560

#define DRIVE_SPEED 150 // 0..255

#define ENA                 5   // PWM Motor controls
#define ENB                 4   // PWM Motor controls
// on the Mega LED_BUILTIN is PIN D13

// Serial3 on pins 15 (RX) and 14 (TX).  <- used for wifi, 115200 baud
// Serial2 on pins 17 (RX) and 16 (TX), 
// Serial1 on pins 19 (RX) and 18 (TX),  <- used for interrrupt
#define SEONSOR_ODOMETRY_LEFT_PIN   18 // interrupt 5
#define SEONSOR_ODOMETRY_RIGHT_PIN  19 // interrupt 4
#define OLED_RESET 20 
#define PWR_L298            33  // enable motor controller
#define IN1                 34   // Motor 1
#define IN2                 35   // Motor 1
#define IN3                 36   // Motor 2
#define IN4                 37   // Motor 2
int L298Npins[7] = {IN1, IN2, IN3, IN4, ENA, ENB, PWR_L298};
#define PIN_AXISARM_ROTATE    45   // Servo 1 - 84 deg mid
#define PIN_AXISARM_PITCH     44   // Servo 2 - 82 deg mid
#define SEONSOR_VOLTAGE_PIN  A11

// Interrupt methods prototypes
void Drive_Odo_Left_Interrupt(); 
void Drive_Odo_Right_Interrupt();

// Callback methods prototypes
void PRINT_DEBUG_Callback(); 
void AxisArm_setAxis_Callback();
void TankDrive_Drive_Callback(); 
void Display_Draw_Callback();
void Comms_RunWebInterfaceCallback(); 
void Measure_Sensor_Voltage_Callback(); 
void Measure_Sensor_Voltage_Callback_2(); 
void TankDrive_Drive_Direction_Callback(); 
void TankDrive_Stop_Callback(); 

// Tasks
Task tPRINT_DEBUG (10000, TASK_FOREVER, &PRINT_DEBUG_Callback); 
Task tAxisArm_setAxis (160, TASK_FOREVER, &AxisArm_setAxis_Callback);  
Task tTankDrive_Drive_Direction (80, TASK_FOREVER, &TankDrive_Drive_Direction_Callback);  
Task tDisplay_Draw (80, TASK_FOREVER, &Display_Draw_Callback); 
Task tComms_RunWebInterface(20, TASK_FOREVER, &Comms_RunWebInterfaceCallback);
Task tMeasure_Sensor_Voltage (9000, TASK_FOREVER, &Measure_Sensor_Voltage_Callback); 



AxisArm axisArm;
Comms comms;
Display disp;
Memory memory;
TankDrive drive;
Scheduler scheduler;

// ------------------- SETUP ------------------
void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);  
  
  //pinMode(SEONSOR_ODOMETRY_LEFT_PIN, INPUT_PULLUP); 
  //pinMode(SEONSOR_ODOMETRY_RIGHT_PIN, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(SEONSOR_ODOMETRY_LEFT_PIN), Drive_Odo_Left_Interrupt, RISING );
  attachInterrupt(digitalPinToInterrupt(SEONSOR_ODOMETRY_RIGHT_PIN), Drive_Odo_Right_Interrupt, RISING );
  
  Serial.begin(9600);
  comms.print("setup begin\r\n");
  
  //scheduler.init();
  scheduler.addTask(tPRINT_DEBUG);
  scheduler.addTask(tAxisArm_setAxis);
  scheduler.addTask(tTankDrive_Drive_Direction);
  scheduler.addTask(tDisplay_Draw);
  scheduler.addTask(tComms_RunWebInterface);
  scheduler.addTask(tMeasure_Sensor_Voltage);
  tPRINT_DEBUG.enable();  
  tAxisArm_setAxis.enable();  
  tTankDrive_Drive_Direction.enableDelayed(500);
  tDisplay_Draw.enableDelayed(1500); 
  tComms_RunWebInterface.enableDelayed(1000);
  tMeasure_Sensor_Voltage.enableDelayed(500);
  
  comms.print("\nsetup end\r\n");

}


// -------------------- LOOP ------------------
void loop(void)
{
  
  scheduler.execute();

}
// ------------------ INTERRUPTS -------------
void Drive_Odo_Left_Interrupt() {  
  memory.drive.LEFT_TICKS++;
}
void Drive_Odo_Right_Interrupt() {  
   memory.drive.RIGHT_TICKS++;
}

// ------------------ CALLBACKS ---------------
// ------------------ PRINT_DEBUG
void PRINT_DEBUG_Callback() {  
  Serial.print("\nmemory.drive.LEFT_TICKS:  ");
  Serial.print(memory.drive.LEFT_TICKS);
  Serial.print(" memory.drive.RIGHT_TICKS: ");  
  Serial.print(memory.drive.RIGHT_TICKS);  
  Serial.println();
  Serial.print("memory.drive.MOTOR_SPEED_L: ");
  Serial.print(memory.drive.MOTOR_SPEED_L);
  Serial.print(" memory.drive.MOTOR_SPEED_R: ");
  Serial.print(memory.drive.MOTOR_SPEED_R);
  Serial.print("\n");
}
// ------------------ AxisArm
void AxisArm_setAxis_Callback() {  
  if (!axisArm._enabled){
    axisArm.enable(PIN_AXISARM_ROTATE, PIN_AXISARM_PITCH);
  }else{
    if(memory.axis.AXIS_ROTATE){
      axisArm.setAxis(0, memory.getAxisRotateAngle(), 15);
    }else{
      axisArm.stop(0);
    }
    if(memory.axis.AXIS_PITCH){
      axisArm.setAxis(1, memory.getAxisPitchAngle(), 15);
    }else{      
      axisArm.stop(1);
    }    
  }  
}


// ------------------ TankDrive
void TankDrive_Drive_Direction_Callback() {  
  if (!drive._enabled){
    drive.enable(&L298Npins);
  }else{    

    // -----------   adjust right motor speed by odometry    
    if(memory.drive.LEFT_TICKS > 100 || memory.drive.RIGHT_TICKS > 100){
      if (abs(memory.drive.LEFT_TICKS - memory.drive.RIGHT_TICKS) > 5){
        if (memory.drive.LEFT_TICKS > memory.drive.RIGHT_TICKS){
          memory.drive.MOTOR_SPEED_R += 5;
        }else{
          memory.drive.MOTOR_SPEED_R -= 5;
        }
      }
      noInterrupts();
      memory.drive.LEFT_TICKS = 0;
      memory.drive.RIGHT_TICKS = 0;
      interrupts();
    }
    // ----------- \ adjust speed by odometry
    
    drive.setDirection(memory.getDriveDirection());
    if(memory.comms.COMMS_WEB_CYCLE_SINCE_MSG > 50){
      tTankDrive_Drive_Direction.set(TASK_IMMEDIATE, TASK_FOREVER, &TankDrive_Stop_Callback);
      tTankDrive_Drive_Direction.delay(200);
    }
  }
}
void TankDrive_Stop_Callback() {  
  if (!drive._enabled){
    drive.enable(&L298Npins);
  }else{      
    drive.disable();
    tTankDrive_Drive_Direction.set(80, TASK_FOREVER, &TankDrive_Drive_Direction_Callback);
  }
}

// ------------------ Display
void Display_Draw_Callback() {  
  if (!disp._enabled){
    disp.enable(OLED_RESET);
  }else{    
    disp.drawOLED();
  }
    
}

// ------------------ Comms
void Comms_RunWebInterfaceCallback() {  
  if (!comms._enabled){
    comms.enableWifi();
  }else{      
    comms.runWebInterface();
  }
}

/*****************************************************************\
  Function:     Measure_Sensor_Voltage_Callback  
  Description:  Measures the Voltage. 
  Parameter:    global - SEONSOR_VOLTAGE_PIN need
  Return:       measured Voltage
\****************************************************************/
#define NUM_VOLTAGE_SAMPLES 100
int sample_sum = 0;
unsigned char sample_count = 0;
void Measure_Sensor_Voltage_Callback() {  

  tMeasure_Sensor_Voltage.setInterval (10);  
  sample_sum += analogRead(SEONSOR_VOLTAGE_PIN);
  sample_count++;  
  if (sample_count >= NUM_VOLTAGE_SAMPLES){  
    tMeasure_Sensor_Voltage.setCallback(&Measure_Sensor_Voltage_Callback_2);
  }
}

void Measure_Sensor_Voltage_Callback_2() {  
  // voltage multiplied by 11 when using voltage divider that divides by 11  ->  11.13 is calibrated
  memory.setSensorVoltage( ( ((float)sample_sum / (float) NUM_VOLTAGE_SAMPLES * 5.1) / 1024.0 ) * 11.129 );
  sample_sum = 0;
  sample_count = 0;
  tMeasure_Sensor_Voltage.set(9000, TASK_FOREVER, &Measure_Sensor_Voltage_Callback);
}


//String UpTime()
//{  
//  long currentmillis=millis();
//  long days=0;
//  long hours=0;
//  long mins=0;
//  long secs=0;
//  secs = currentmillis/1000; //convect milliseconds to seconds
//  mins=secs/60; //convert seconds to minutes
//  hours=mins/60; //convert minutes to hours
//  days=hours/24; //convert hours to days
//  secs=secs-(mins*60); //subtract the coverted seconds to minutes in order to display 59 secs max
//  mins=mins-(hours*60); //subtract the coverted minutes to hours in order to display 59 minutes max
//  hours=hours-(days*24); //subtract the coverted hours to days in order to display 23 hours max
//  
//  return String(days) + "d" + String(hours) + "h" + String(mins) + "m" + String(secs) + "s";
//}
