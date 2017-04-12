//#define PRINT_DBG

#include <Stepper.h>
#include "PVision.h"
#include "Relay.h"

#define CAM_X_MAX 768
#define CAM_X_CENT (CAM_X_MAX / 2.0)
#define CAM_Y_MAX 768
#define BLOB_SEEKING_X_THRESHOLD (CAM_X_CENT - 25)

#define MAX_RPM 60
#define MAX_STEP 10
#define DEADBAND 0
#define MAX_ROTATIONAL_OFFSET_DEG 75
#define TICKS_PER_REV 320
#define DEG_PER_TICK ((double) 360 / TICKS_PER_REV)
#define TIMEOUT_MS 5000

#define ENC_A  0
#define ENC_B  1
#define STEPPER_A 8
#define STEPPER_B 9
#define STEPPER_C 10
#define STEPPER_D 11

// settings
bool isVarRpm = true;
bool isVarStep = true;
bool isMotionLimited = true;

// state
bool isSeekingBlob;
bool isBlobVisible;

double angle = 0;
unsigned long inactiveStart = 0;

PVision cam;
int camX;

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

#ifndef PRINT_DBG
// period ~0.033 s
ISR(TIMER3_OVF_vect) {
  Serial.write(buf, sizeof(buf));
  Serial.flush();
}
#endif

void doEncoder() {
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
   */ 
  if (digitalRead(ENC_A) == digitalRead(ENC_B)) {
    angle += DEG_PER_TICK;

    (*encPos)++;
    *encPos %= TICKS_PER_REV;
  } else {
    angle -= DEG_PER_TICK;

    if (*encPos == 0) {
      *encPos = TICKS_PER_REV;
    } else {
      (*encPos)--;
      *encPos %= TICKS_PER_REV;
    }
  }
#ifdef PRINT_DBG
  Serial.print (*encPos, DEC);
  Serial.print(" ");
  Serial.println(angle);
#endif
}

void rotateBase(int rpm, int step) {
  if ((isBlobVisible || isSeekingBlob) &&
      (!isMotionLimited ||
      (!(angle < -MAX_ROTATIONAL_OFFSET_DEG && step > 0) &&
      !(angle > MAX_ROTATIONAL_OFFSET_DEG && step < 0)))) {
    motor.setSpeed(rpm);
    motor.step(step);
  } else {
    digitalWrite(STEPPER_A, LOW);
    digitalWrite(STEPPER_B, LOW);
    digitalWrite(STEPPER_C, LOW);
    digitalWrite(STEPPER_D, LOW);
  }
}

void resetTimeout() {
  motorsAndFansRelay.Close();
  fogMachineRelay.Close();
  inactiveStart = millis();
}

void stopIfTimeout() {
  if (millis() - inactiveStart > TIMEOUT_MS) {
    motorsAndFansRelay.Open();
    fogMachineRelay.Open();
  }
}

void setup() {
#ifndef PRINT_DBG
  // timer interrupt setup
  noInterrupts();
  TCCR3A = 0;
  TCCR3B = 0;

  TIMSK3 |= (1 << TOIE3);  // enable interrupt handler

  // 8 prescaler--increment TNCT3 every 8*(6.25e-8) s
  TCCR3B |= (1 << CS31);

  interrupts();
  // end the timer interrupt setup
#endif

  // relay box setup - everything on by default
  motorsAndFansRelay.Close();
  fogMachineRelay.Close();
  camRelay.Close();
  // end relay box setup

  cam.Init();

  memset(buf, 0, sizeof(buf));

  motor.setSpeed(MAX_RPM);

  pinMode(ENC_A, INPUT); 
  digitalWrite(ENC_A, HIGH);       // turn on pull-up resistor
  pinMode(ENC_B, INPUT); 
  digitalWrite(ENC_B, HIGH);       // turn on pull-up resistor

  attachInterrupt(2, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
  Serial.begin (9600);
  Serial.println("start");                // a personal quirk
  resetTimeout();
} 

void loop() {
  cam.Read();
  isBlobVisible = cam[0].visible;
  stopIfTimeout();

  if (isBlobVisible) {
    isSeekingBlob = false;
    resetTimeout();

    camX = cam[0].y - CAM_X_CENT;  // ports for X and Y are mixed up - need to fix
    *camY = (uint16_t) (CAM_Y_MAX - cam[0].x);

    if (abs(camX) > DEADBAND) {
      mult = (double) abs(camX) / CAM_X_CENT;
      rpm = isVarRpm ? mult * MAX_RPM : MAX_RPM;
      step = isVarStep ? mult * MAX_STEP : MAX_STEP;

      if (camX > 0) {
        step *= -1;
      }

      rotateBase(rpm, step);
    }

#ifdef PRINT_DBG
    Serial.print(camX);
    Serial.print(" ");
    Serial.print(*camY);
    Serial.print(" ");
    Serial.print(cam[0].dist);
    Serial.println();
#endif  
  } else {
    isSeekingBlob = abs(camX) > BLOB_SEEKING_X_THRESHOLD;
    rotateBase(MAX_RPM, cam[0].dty > 0 ? -MAX_STEP : MAX_STEP);
  }
}

