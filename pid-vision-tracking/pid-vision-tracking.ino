#include <Stepper.h>
#include <PVision.h>
#include <PID_v1.h>

#define CAM_MAX 500
#define MAX_RPM 10
#define MAX_STEP 5
#define DEADBAND 10

PVision cam;
double pos = 0;
bool hasBlob;

Stepper motor(360, 8, 9, 10, 11);
double mult = 0;
long rpm;
int step;

double setpoint = 0;
PID pidCtrl(&pos, &mult, &setpoint, 0.1, 0.0, 0.0, DIRECT);

void setup() {
  cam.init();
  motor.setSpeed(MAX_RPM);

  pidCtrl.SetMode(AUTOMATIC);
  pidCtrl.SetOutputLimits(0, 1);

  Serial.begin(9600);
}

void loop() {
  hasBlob = cam.read() & BLOB1;

  if (hasBlob) {
    pos = cam.Blob1.Y - (CAM_MAX / 2);  // ports for X and Y are mixed up - need to fix

    if (abs(pos) > DEADBAND) {
      pidCtrl.Compute();

      rpm = mult * MAX_RPM;
      step = mult * MAX_STEP;

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

  if (!hasBlob || abs(pos) <= DEADBAND) {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);

    Serial.println("no blob detected");
  }
}

