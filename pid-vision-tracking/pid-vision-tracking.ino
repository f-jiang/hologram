#include <Stepper.h>
#include <PVision.h>
#include <PID_v1.h>

#define CAM_MAX 760
#define CAM_MIN 0
#define CAM_CENT ((CAM_MAX + CAM_MIN) / 2.0)
#define MAX_RPM 60
#define MAX_STEP 10
#define DEADBAND 0

PVision cam;
double pos = 0;
bool hasBlob;

Stepper motor(360, 8, 9, 10, 11);
double mult = 0;
long rpm;
int step;

double setpoint = CAM_CENT;
PID pidCtrl(&pos, &mult, &setpoint, 0.1, 0.0, 0.0, DIRECT);

void setup() {
  cam.init();
  motor.setSpeed(MAX_RPM);

  pidCtrl.SetMode(AUTOMATIC);
  pidCtrl.SetOutputLimits(-100, 100);

  Serial.begin(9600);
  Serial.println("ready");
}

void loop() {
  hasBlob = cam.read() & BLOB1;

  if (hasBlob) {
    pos = cam.Blob1.Y;  // ports for X and Y are mixed up - need to fix

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
}



