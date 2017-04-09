//#define PRINT_DBG

#include <Stepper.h>
#include <PVision.h>

#define CAM_X_MAX 1024
#define CAM_X_CENT (CAM_X_MAX / 2.0)
#define CAM_Y_MAX 768

#define MAX_RPM 60
#define MAX_STEP 10
#define DEADBAND 0
#define MAX_ROTATIONAL_OFFSET_DEG 75
#define TICKS_PER_REV 320
#define DEG_PER_TICK ((double) 360 / TICKS_PER_REV)

#define ENC_A  0
#define ENC_B  1
#define STEPPER_A 8
#define STEPPER_B 9
#define STEPPER_C 10
#define STEPPER_D 11

#define RELAY_MOTORS_AND_FANS 4  // N/O
#define RELAY_FOG_MACH        5  // N/O
#define RELAY_CAMERA          6  // N/C

bool varRpm = true;
bool varStep = true;

double angle = 0;

PVision cam;
int pos;
bool hasBlob;

Stepper motor(360, STEPPER_A, STEPPER_B, STEPPER_C, STEPPER_D);
long rpm;
int step;

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
  // does positive |step| increase or decrease |angle|?
  if (!(angle < -MAX_ROTATIONAL_OFFSET_DEG && step < 0) &&
      !(angle > MAX_ROTATIONAL_OFFSET_DEG && step > 0)) {
    motor.setSpeed(rpm);
    motor.step(step);
  }
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
  pinMode(RELAY_MOTORS_AND_FANS, OUTPUT);
  pinMode(RELAY_FOG_MACH, OUTPUT);
  pinMode(RELAY_CAMERA, OUTPUT);
  digitalWrite(RELAY_MOTORS_AND_FANS, LOW);
  digitalWrite(RELAY_FOG_MACH, LOW);
  digitalWrite(RELAY_CAMERA, HIGH);
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

      rotateBase(rpm, step);
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




