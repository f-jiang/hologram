//#define PRINT_DBG

#include <Stepper.h>
#include <PVision.h>
#include "Relay.h"

#define CAM_X_MAX 1024
#define CAM_X_CENT (CAM_X_MAX / 2.0)
#define CAM_Y_MAX 768

#define MAX_RPM 60
#define MAX_STEP 10
#define DEADBAND 0
#define TICKS_PER_REV 320

#define ENC_A  0
#define ENC_B  1
#define STEPPER_A 8
#define STEPPER_B 9
#define STEPPER_C 10
#define STEPPER_D 11

bool varRpm = true;
bool varStep = true;

PVision cam;
int pos;
bool hasBlob;

Stepper motor(360, STEPPER_A, STEPPER_B, STEPPER_C, STEPPER_D);
long rpm;
int step;

Relay motorsAndFansRelay(4, Relay::Mode::NORMALLY_OPEN);
Relay fogMachineRelay(5, Relay::Mode::NORMALLY_OPEN);
Relay camRelay(6, Relay::Mode::NORMALLY_CLOSED);

double mult;

uint8_t buf[4];
uint16_t *encPos = (uint16_t *)buf;
uint16_t *camY = (uint16_t *)(buf + 2);

// period ~0.033 s
ISR(TIMER3_OVF_vect) {
  Serial.write(buf, sizeof(buf));
  Serial.flush();
}

void doEncoder() {
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
   */ 
  if (digitalRead(ENC_A) == digitalRead(ENC_B)) {
    (*encPos)++;
    *encPos %= TICKS_PER_REV;
  } else if (*encPos == 0) {
    *encPos = TICKS_PER_REV;
  } else {
    (*encPos)--;
    *encPos %= TICKS_PER_REV;
  }

#ifdef PRINT_DBG
  Serial.println (*encPos, DEC);
#endif
}

void setup() {
  // timer interrupt setup
  noInterrupts();
  TCCR3A = 0;
  TCCR3B = 0;

  TIMSK3 |= (1 << TOIE3);  // enable interrupt handler

  // 8 prescaler--increment TNCT3 every 8*(6.25e-8) s
  TCCR3B |= (1 << CS31);

  interrupts();
  // end the timer interrupt setup

  // relay box setup - everything on by default
  motorsAndFansRelay.Close();
  fogMachineRelay.Close();
  camRelay.Close();
  // end relay box setup

  cam.init();

  memset(buf, 0, sizeof(buf));

  motor.setSpeed(MAX_RPM);

  pinMode(ENC_A, INPUT); 
  digitalWrite(ENC_A, HIGH);       // turn on pull-up resistor
  pinMode(ENC_B, INPUT); 
  digitalWrite(ENC_B, HIGH);       // turn on pull-up resistor

  attachInterrupt(2, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
  Serial.begin (9600);
  Serial.println("start");                // a personal quirk
} 

void loop(){
  hasBlob = cam.read() & BLOB1;

  if (hasBlob) {
    pos = cam.Blob1.Y - CAM_X_CENT;  // ports for X and Y are mixed up - need to fix
    *camY = (uint16_t) (CAM_Y_MAX - cam.Blob1.X);

    if (abs(pos) > DEADBAND) {
      mult = (double) abs(pos) / CAM_X_CENT;
      rpm = varRpm ? mult * MAX_RPM : MAX_RPM;
      step = varStep ? mult * MAX_STEP : MAX_STEP;

      if (pos > 0) {
        step *= -1;
      }

      motor.setSpeed(rpm);
      motor.step(step);
    }

#ifdef PRINT_DBG
    Serial.print(cam.Blob1.Y);
    Serial.print(" ");
    Serial.print(cam.Blob1.X);
    Serial.print(" ");
    Serial.print(cam.Blob1.Size);
    Serial.println();
#endif  
  }

  if (!hasBlob || abs(pos) <= DEADBAND) {
    digitalWrite(STEPPER_A, LOW);
    digitalWrite(STEPPER_B, LOW);
    digitalWrite(STEPPER_C, LOW);
    digitalWrite(STEPPER_D, LOW);
  }
}




