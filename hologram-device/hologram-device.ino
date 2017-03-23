#define PRINT_DBG

#include <Stepper.h>
#include <PVision.h>

#define CAM_MAX 760
#define CAM_MIN 0
#define CAM_CENT ((CAM_MAX + CAM_MIN) / 2.0)
#define MAX_RPM 60
#define MAX_STEP 10
#define DEADBAND 0
#define TICKS_PER_REV 320

#define ENC_A  0
#define ENC_B  1

bool varRpm = true;
bool varStep = true;

PVision cam;
int pos;
bool hasBlob;

Stepper motor(360, 8, 9, 10, 11);
long rpm;
int step;

double mult;

uint16_t encPos = 0;
uint16_t camY = 0;
uint8_t buf[4];

void serialWrite() {
  memcpy(buf, &encPos, sizeof(uint16_t));
  memcpy(buf + 2, &camY, sizeof(uint16_t));
  Serial.write(buf, sizeof(buf));
  delay(5);
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

#ifdef PRINT_DBG
  Serial.println (encPos, DEC);
#endif
}

void setup() {
  cam.init();
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
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }
}

