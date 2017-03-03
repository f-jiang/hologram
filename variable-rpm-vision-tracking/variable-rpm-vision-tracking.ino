#include <PVision.h>
#include <Stepper.h>

#define CAM_MAX 500
#define MAX_RPM 60
#define STEP_DEG 5
#define THRESHOLD 30

PVision cam;

Stepper motor(360, 8, 9, 10, 11); // right now 8, 9, 10, and 11 are the ports
long rpm;
int pos;

void setup() {
  cam.init();
  Serial.begin(9600);
}

void loop() {
  if (cam.read() & BLOB1) {
    pos = cam.Blob1.Y - (CAM_MAX / 2);  // ports for X and Y are mixed up - need to fix

    if (abs(pos) > THRESHOLD) {
      rpm = ((double) abs(pos) / (CAM_MAX / 2)) * MAX_RPM;
      motor.setSpeed(rpm);
      motor.step(pos > 0 ? STEP_DEG: -STEP_DEG);  // which sign is clockwise?
    }

    Serial.print(cam.Blob1.Y);
    Serial.print(" ");
    Serial.print(cam.Blob1.X);
    Serial.print(" ");
    Serial.print(cam.Blob1.Size);
    Serial.println();
  }

  Serial.println("---");
}

