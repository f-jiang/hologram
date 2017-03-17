//#define PID_VISION_TRACKING

#ifdef PID_VISION_TRACKING
#include <Stepper.h>
#include <PVision.h>
#include <PID_v1.h>
#else
#include <Stepper.h>
#include <PVision.h>
#endif

#define CAM_MAX 760
#define CAM_MIN 0
#define CAM_CENT ((CAM_MAX + CAM_MIN) / 2.0)
#define MAX_RPM 60
#define MAX_STEP 10
#define DEADBAND 0
#define TICKS_PER_REV 320

#define ENC_A  2
#define ENC_B  3

#ifdef PID_VISION_TRACKING
PVision cam;
double pos = 0;
bool hasBlob;

Stepper motor(360, 8, 9, 10, 11);
double mult = 0;
long rpm;
int step;

double setpoint = CAM_CENT;
PID pidCtrl(&pos, &mult, &setpoint, 0.1, 0.0, 0.0, DIRECT);
#else
bool varRpm = true;
bool varStep = true;

PVision cam;
int pos;
bool hasBlob;

Stepper motor(360, 8, 9, 10, 11);
long rpm;
int step;

double mult;
#endif

uint16_t encPos = 0;
uint16_t camY = 0;
uint8_t buf[4];

void serialWrite() {
  memcpy(buf, &encPos, sizeof(uint16_t));
  memcpy(buf + 2, &camY, sizeof(uint16_t));
  Serial.write(buf, sizeof(buf));
}

void doEncoder() {
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
   */ 
  if (digitalRead(ENC_A) == digitalRead(ENC_B)) {
    encPos++;
    encPos %= TICKS_PER_REV;
  } else if (encPos == 0) {
    encPos = TICKS_PER_REV;
  } else {
    encPos--;
    encPos %= TICKS_PER_REV;
  }

  serialWrite();

  Serial.println (encPos, DEC);
}

void setup() {
#ifdef PID_VISION_TRACKING
  cam.init();
  motor.setSpeed(MAX_RPM);

  pidCtrl.SetMode(AUTOMATIC);
  pidCtrl.SetOutputLimits(-100, 100);
#else
  cam.init();
  motor.setSpeed(MAX_RPM);
#endif

  pinMode(ENC_A, INPUT); 
  digitalWrite(ENC_A, HIGH);       // turn on pull-up resistor
  pinMode(ENC_B, INPUT); 
  digitalWrite(ENC_B, HIGH);       // turn on pull-up resistor

  attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
  Serial.begin (9600);
  Serial.println("start");                // a personal quirk
} 

void loop(){
#ifdef PID_VISION_TRACKING
  hasBlob = cam.read() & BLOB1;

  if (hasBlob) {
    pos = cam.Blob1.Y;  // ports for X and Y are mixed up - need to fix
    camY = cam.Blob1.X;
    serialWrite();

    if (abs(pos) > DEADBAND) { 
      pidCtrl.Compute();
      mult /= 100.0;

      rpm = abs(mult) * MAX_RPM;
      step = mult * MAX_STEP;

      if (pos > CAM_CENT) {
        step *= -1;
      }

//      motor.setSpeed(rpm);
//      motor.step(step);  // which sign is clockwise?
    }
  }

  if (!hasBlob || abs(pos) <= DEADBAND) {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }

  Serial.print(cam.Blob1.Y);
  Serial.print(" ");
  Serial.print(pos);
  Serial.print(" ");
  Serial.print(cam.Blob1.X);
  Serial.print(" ");
  Serial.print(cam.Blob1.Size);
  Serial.print(" ");
  Serial.print(setpoint);
  Serial.print(" ");
  Serial.print(mult);
  Serial.print(" ");            
  Serial.print(rpm);
  Serial.print(" ");            
  Serial.print(step);
  Serial.print(" ");            
  Serial.println();
#endif
#else
  hasBlob = cam.read() & BLOB1;

  if (hasBlob) {
    pos = cam.Blob1.Y - CAM_CENT;  // ports for X and Y are mixed up - need to fix
    camY = cam.Blob1.X;
    serialWrite();

    if (abs(pos) > DEADBAND) {
      mult = (double) abs(pos) / CAM_CENT;
      rpm = varRpm ? mult * MAX_RPM : MAX_RPM;
      step = varStep ? mult * MAX_STEP : MAX_STEP;

      if (pos > 0) {
        step *= -1;
      }

      motor.setSpeed(rpm);
      motor.step(step);
    }

    Serial.print(cam.Blob1.Y);
    Serial.print(" ");
    Serial.print(cam.Blob1.X);
    Serial.print(" ");
    Serial.print(cam.Blob1.Size);
    Serial.println();  
  }

  if (!hasBlob || abs(pos) <= DEADBAND) {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }
#endif
}

