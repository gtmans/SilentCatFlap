// SilentCatFlap 2025 gtmans github
/*
parts:
- M5 Atom light
- 28BYJ-48 with ULN2003
- microswitch

hardware setup:
to read serial port 33 on atom to detect +3, -3 or 0v
GNDIN ----------+------------------ GND     catflap black to M5 GND
               10K
                +-- |<-DIODE --+--- GPIO33  (serial pin)
               10K            10K
UNKNWN*---------+              +--- 3v3     *catflap red to M5 +3.3v

pin connectors:
XX      [25] [GND]             ← microswitch
X IN1   [22]                   ← steppenmotor 28BYJ blue
X IN2   [19]                   ← steppenmotor 28BYJ pink
X IN3   [23]                   ← steppenmotor 28BYJ yellow
X IN4   [21]                   ← steppenmotor 28BYJ orange
*/

#include    "M5Atom.h"
const int   analogPin   = 33; 
bool        flapopen,lastflap,flapchg;
bool        triggered   = false;
int         sensorValue,flap,flapRaw,flapstat;
const int   samples     = 3;
int         dimcnt;

#define     STOPSWITCH_PIN  25
int homing_count;

#include    <Stepper.h>
#define     IN1 22
#define     IN2 19
#define     IN3 23
#define     IN4 21
const int   STEPS_PER_REV   = 2039;
const int   ANGLE_140_STEPS = 793;
const int   DEG10 = ANGLE_140_STEPS/14;
int         degcnt,degsteps,stp,stpmax=30;//18 steps

Stepper myStepper   (STEPS_PER_REV, IN1, IN3, IN2, IN4);

void setup() {
  M5.begin          (true, false, true);
  delay             (50);
  myStepper.setSpeed(12);// 10=OK, 14 Not working
  Serial.println    ();
  Serial.println    (__FILE__);
  pinMode           (STOPSWITCH_PIN, INPUT_PULLUP); // schakelaar naar GND
  pinMode           (IN1, OUTPUT);
  pinMode           (IN2, OUTPUT);
  pinMode           (IN3, OUTPUT);
  pinMode           (IN4, OUTPUT);
  homing            ();
//showoff after power on: remove if not wanted
  openflap          ();
  delay             (2000);
  closeflap         ();
//end of demo
}

void redlights(){
  for (int i=0;i<6;i++){
    M5.dis.drawpix    (0, 0xff0000);//red
    delay (500);
    M5.dis.drawpix    (0, 0x000000);
    delay (500);
  }
}
void openflap(){
  Serial.println  ("openflap()");
  while           (digitalRead(STOPSWITCH_PIN)!=LOW){redlights();homing();}// not fully open safe escape
  Serial.println  (String(stpmax*DEG10)+" stappen vooruit!");
  myStepper.step  (-stpmax*DEG10);
  releaseMotor    ();
  flapstat    =   2;
  flapopen    =   true;
  M5.dis.drawpix  (0, 0x00ff00);//green
  dimcnt      =   0;
}

void closeflap(){
  Serial.println  ("closeflap()");
  stp         =   stpmax;
  int stapjes =   0;
  while (digitalRead(STOPSWITCH_PIN)!=LOW){
  //Serial.print  (stp);
  //Serial.print  ("-");
    stp--;
    if (stp<0)    {break;}
    myStepper.step(DEG10);
    stapjes +=    DEG10;
  }  
  Serial.println  (" "+String(stapjes)+" stappen terug!");
  releaseMotor    ();
  flapstat    =   1;
  flapopen    =   false;
  M5.dis.drawpix  (0, 0xff0000);//red
  dimcnt      =   0;
}

void homing(){            // naar referentiepunt
  M5.dis.drawpix    (0, 0x0000ff);//blue
  Serial.print            ("homing()");
  homing_count     =      0;
  while(digitalRead       (STOPSWITCH_PIN) == HIGH && homing_count < stpmax*2) {
    myStepper.step        (DEG10/2);
    homing_count++;
    delay(2);
  }
  releaseMotor    ();
  Serial.println          (digitalRead(STOPSWITCH_PIN)==LOW ? "gevonden!" : "niet gevonden!");
  Serial.println          ("homing_count:"+String(homing_count/2));
  if (digitalRead         (STOPSWITCH_PIN)==LOW){flapopen=false;}else{flapopen=true;}
}

void loop() {
  M5.update();            // Read the press state of the key.
  if (M5.Btn.wasPressed()) {
    Serial.print          (flapstat);
    if (digitalRead       (STOPSWITCH_PIN)==LOW){//closed
      flapstat    =       2;
    } else  {
      flapstat    =       1;
    }  
    flapchg       =       true;
    Serial.println        (flapstat);
  } else {readFlap();}
  if (flapchg){
    Serial.println        ("flapopen:"+String(flapopen)+" flapstat:"+String(flapstat));  
    switch (flapstat){
    case 0:  // none
      //M5.dis.drawpix    (0, 0x0000ff);//blue
        Serial.print      ("0V");
        break;
    case 1:  // do close
      //M5.dis.drawpix    (0, 0xff0000);//red
        Serial.println    ("CLOSE -3v send CLOSE CMD");
        closeflap         ();
        break;
    case 2:  // do open
      //M5.dis.drawpix    (0, 0x00ff00);//green
        Serial.println    ("OPEN +3v send OPEN CMD");
        openflap          ();
        break;
    }
    Serial.println        ("flapopen:"+String(flapopen)+" flapstat:"+String(flapstat)); 
  }
  if (triggered){
    M5.dis.drawpix        (0, 0x800080);//pink
    Serial.print          ("Waiting ...");
    delay                 (15000);
    Serial.println        ("Auto close flap after 15 seconds");
    triggered   =         false;
    closeflap             ();
    M5.dis.drawpix        (0, 0xff0000);//close
  } else                  {delay(100);}
  dimcnt++;
  if (dimcnt>600)         {dimcnt=0;M5.dis.drawpix(0, 0x000000);}//prox 1 minute
}

void releaseMotor() {
  digitalWrite            (IN1, LOW);
  digitalWrite            (IN2, LOW);
  digitalWrite            (IN3, LOW);
  digitalWrite            (IN4, LOW);
}

void readFlap() {
  lastflap    =           flapopen;
  flapRaw     =           0;
  for (int i  = 0; i < samples; i++) {
    flapRaw  += analogRead(analogPin);
    delay                 (3);
  }  
  flap        =           flapRaw/samples;
  if         (flap<500)   {//-3V
    flapopen  =           true; //do open
    flapstat  =           2;    //open
    triggered =           true;
  } else if  (flap>2200) {//3V
    flapopen  =           false;//do close
    flapstat  =           1;    //close
  } else {
    flapstat  =           0;    //none
  }
  if (lastflap!=flapopen) {flapchg=true;}else{flapchg=false;}
}
