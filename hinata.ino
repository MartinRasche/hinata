

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

#include <AxisArm.h>
#include <Comms.h>
#include <Display.h>
#include <Memory.h>
#include <TankDrive.h>
#include <TaskScheduler.h>

// keep in mind
// The Arduino Mega has three additional serial ports: 
// Serial1 on pins 19 (RX) and 18 (TX),  <- used for wifi, 9600 baud
// Serial2 on pins 17 (RX) and 16 (TX), 
// Serial3 on pins 15 (RX) and 14 (TX).  
// https://www.arduino.cc/en/Hacking/PinMapping2560

#define ENA                 5   // PWM Motor controls
#define ENB                 6   // PWM Motor controls
#define IN1                 4   // Motor 1
#define IN2                 7   // Motor 1
#define IN3                 8   // Motor 2
#define IN4                 9   // Motor 2
int L298Npins[6] = {IN1, IN2, IN3, IN4, ENA, ENB};
// on the Mega LED_BUILTIN is PIN D13 
#define OLED_RESET 20 
#define PIN_AXISARM_ROTATE    45   // Servo 1 - 84 deg mid
#define PIN_AXISARM_PITCH     44   // Servo 2 - 82 deg mid
#define SEONSOR_VOLTAGE_PIN  A11


// Callback methods prototypes
void AxisArm_setAxis_Callback();
void TankDrive_Drive_Callback(); 
void Display_Draw_Callback();
void Comms_RunWebInterfaceCallback(); 
void Measure_Sensor_Voltage_Callback(); 
void Measure_Sensor_Voltage_Callback_2(); 
void TankDrive_Drive_Callback(); 
void TankDrive_Stop_Callback(); 

// Tasks
Task tAxisArm_setAxis (500, TASK_FOREVER, &AxisArm_setAxis_Callback);  
Task tTankDrive_Drive (80, TASK_FOREVER, &TankDrive_Drive_Callback);  
Task tDisplay_Draw (40, TASK_FOREVER, &Display_Draw_Callback); 
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
  
  Serial.begin(9600);
  comms.print("setup begin\r\n");

  //scheduler.init();
  scheduler.addTask(tAxisArm_setAxis);
  scheduler.addTask(tTankDrive_Drive);
  scheduler.addTask(tDisplay_Draw);
  scheduler.addTask(tComms_RunWebInterface);
  scheduler.addTask(tMeasure_Sensor_Voltage);
  tAxisArm_setAxis.enable();  
  tTankDrive_Drive.enableDelayed(500);
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

// ------------------ CALLBACKS ---------------
// ------------------ AxisArm
void AxisArm_setAxis_Callback() {  
  if (!axisArm._enabled){
    axisArm.enable(PIN_AXISARM_ROTATE, PIN_AXISARM_PITCH);
  }else{
    axisArm.setAxis(0, memory.getAxisRotateAngle(), 20);
    axisArm.setAxis(1, memory.getAxisPitchAngle(), 20);
  }
}
// ------------------ TankDrive
void TankDrive_Drive_Callback() {  
  if (!drive._enabled){
    drive.enable(&L298Npins);
  }else{    
    drive.setDirection(memory.getDriveDirection());
    if(memory.comms.COMMS_WEB_CYCLE_SINCE_MSG > 20){
      tTankDrive_Drive.set(TASK_IMMEDIATE, TASK_FOREVER, &TankDrive_Stop_Callback);
      tTankDrive_Drive.delay(200);
    }
  }
}
void TankDrive_Stop_Callback() {  
  if (!drive._enabled){
    drive.enable(&L298Npins);
  }else{      
    drive.stop();
    tTankDrive_Drive.set(20, TASK_FOREVER, &TankDrive_Drive_Callback);
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
  memory.setSensorVoltage( ( ((float)sample_sum / (float) NUM_VOLTAGE_SAMPLES * 5.1) / 1024.0 ) * 11.13 );
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
