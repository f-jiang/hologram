#include <PVision.h>
#include <Stepper.h>

#define CAM_MAX 500
#define MAX_STEP 10
#define THRESHOLD 30

PVision cam;

Stepper motor(360, 8, 9, 10, 11); // right now 8, 9, 10, and 11 are the ports
int step;
int pos;

void setup() {
  cam.init();
  motor.setSpeed(10);
  Serial.begin(9600);
}

void loop() {
  if (cam.read() & BLOB1) {
    pos = cam.Blob1.Y - (CAM_MAX / 2);  // ports for X and Y are mixed up - need to fix

    if (abs(pos) > THRESHOLD) {
      step = ((double) abs(pos) / (CAM_MAX / 2)) * MAX_STEP;
      motor.step(pos > 0 ? step: -step);  // which sign is clockwise?
    }

    Serial.print(cam.Blob1.Y);
    Serial.print(" ");
    Serial.print(cam.Blob1.X);
    Serial.print(" ");
    Serial.print(cam.Blob1.Size);
    Serial.println();
  } else {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }

  Serial.println("---");
}

