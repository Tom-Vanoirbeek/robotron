#include <EEPROM.h>
#include <BMSerial.h>
#include <BMPS2.h>
#include <SPI.h>
#include <Orion.h>

#define DEBUG 1

#define SIMPLEWRIST
//#define CUSTOMPINS

#ifdef CUSTOMPINS
  #define BASE 0
  #define SHOULDER 1
  #define ELBOW 2
  #define WRIST 3
  #define WRISTROT 4
  #define GRIP 5
#else
  #define BASE 5
  #define SHOULDER 4
  #define ELBOW 3
  #define WRIST 2
  #define WRISTROT 1
  #define GRIP 0
#endif

#define DEADZONE 20

#define GREENLED A1
#define REDLED A0

#define KEY 0x4321

//Maximum body offset
#define MAXX 1100
#define MAXY 1800
#define MAXZ 800
#define MAXR 350

#define DEGREE 178

//leg servo pin assignments
const int IKPins[6] = {BASE,SHOULDER,ELBOW,WRIST,WRISTROT,GRIP};
#ifdef SIMPLEWRIST
  //leg dimensions
  const int IKLengths[6] = {0,1535,1330,0,0,0};
  //Start position
  const int StartPos[6] = {0,800,0,0,0,100};
#else
  //leg dimensions
  const int IKLengths[6] = {0,1535,1330,1680,0,0};
  //Start position
  const int StartPos[6] = {1680,800,0,0,0,100};
#endif
//Servo angle offsets
const int ServoAOffsets[6] = {0,500,750,200,0,0};
//Servo normal/reverse flags
const unsigned char reverse[6] = {0,1,1,1,0,1};

unsigned int Rate;

boolean onoff;
boolean heartbeat;
long lastbeat;

boolean wristmode;

int incomingByte = 0;



byte byteRead;
long num1, num2,answer;
boolean mySwitch=false;


void setup() {                
/* Turn the Serial Protocol ON and 
   initialise num1 variables.*/
  Serial.begin(9600);
  num1=0;
 
  Orion.begin();
  Orion.green(true);
  Orion.red(false);
  
  //Set Body Dimensions and initial Leg positions
  unsigned long flags=0;
  for(int i=0;i<6;i++){
    Orion.stopPulse(IKPins[i]);  //stops servo if running and starts reading feedback data if not
    Orion.setSpeed(1600);  //limit maximum servo speed
       
  //Init other system variables
  lastbeat = millis();
  onoff = false;

  Orion.tone(NOTE_C6,100);
  Orion.tone(NOTE_B5,100);
  Orion.tone(NOTE_A5,100);

  wristmode = false;
    }
}

void loop() {
   /*  check if data has been sent from the computer: */
  while (Serial.available()) {
    /* read the most recent byte */
    byteRead = Serial.read();
    
    //listen for numbers between 0-9
    if(byteRead>47 && byteRead<58){
       //num1*10 is needed so that you can recieve 2 ascii signs
         num1=(num1*10)+(byteRead-48);
    } 
  
    
    /*Listen for an equal sign (byte code 61) 
      to calculate the answer and send it back to the
      serial monitor screen 61*/
    if(byteRead==43){
      answer=num1;
   
      Serial.println(answer/10);
      //the engine gets his Positive angle
       Orion.setAngle(GRIP,answer);
       //thanks the command execute will  the angle be send to the motor
   Orion.execute();
      
    /* Listen for the addition sign (byte code 43). This is
       used as a delimiter to help define num1 from num2 */  
    }
    if(byteRead==45){
      answer=-num1;
   
      Serial.println(answer/10);
      //the engine gets his engative angle
       Orion.setAngle(GRIP,answer);
       //thanks the command execute will  the angle be send to the motor
   Orion.execute();
      
   
    }
    //by byteread=42 answer and num1 will be reset
    if(byteRead==42){
      answer=0;
      num1=0;
        }
  }
}
