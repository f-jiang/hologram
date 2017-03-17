#include <Stepper.h>
#include <PVision.h>

#define CAM_MAX 760
#define CAM_MIN 0
#define CAM_CENT ((CAM_MAX + CAM_MIN) / 2.0)
#define RPM 60
#define STEP 10
#define THRESHOLD 0

bool varRpm = true;
bool varStep = true;

PVision cam;
int pos;
bool hasBlob;

Stepper motor(360, 8, 9, 10, 11);
long rpm;
int step;

double mult;

void setup() {
  cam.init();
  motor.setSpeed(RPM);

  Serial.begin(9600);
  Serial.println("ready");
}

void loop() {
  hasBlob = cam.read() & BLOB1;

  if (hasBlob) {
    pos = cam.Blob1.Y - CAM_CENT;  // ports for X and Y are mixed up - need to fix

    if (abs(pos) > THRESHOLD) {
      mult = (double) abs(pos) / CAM_CENT;
      rpm = varRpm ? mult * RPM : RPM;
      step = varStep ? mult * STEP : STEP;

      if (pos > 0) {
        step *= -1;
      }

      motor.setSpeed(rpm);
      motor.step(step);  // which sign is clockwise?
    }

    Serial.print(cam.Blob1.Y);
    Serial.print(" ");
    Serial.print(cam.Blob1.X);
    Serial.print(" ");
    Serial.print(cam.Blob1.Size);
    Serial.println();  
  }

  if (!hasBlob || abs(pos) <= THRESHOLD) {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }
}

