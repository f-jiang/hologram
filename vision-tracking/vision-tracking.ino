#include <PVision.h>
#include <Stepper.h>

#define CAM_MAX 500
#define RPM 10
#define STEP 5
#define THRESHOLD 30

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
}

void loop() {
  hasBlob = cam.read() & BLOB1;

  if (hasBlob) {
    pos = cam.Blob1.Y - (CAM_MAX / 2);  // ports for X and Y are mixed up - need to fix

    if (abs(pos) > THRESHOLD) {
      mult = (double) abs(pos) / (CAM_MAX / 2);
      rpm = varRpm ? mult * RPM : RPM;
      step = varStep ? mult * STEP : STEP;

      if (pos < 0) {
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

    Serial.println("no blob detected");
  }
}

