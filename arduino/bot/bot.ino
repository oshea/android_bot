#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *backLeft = AFMS.getMotor(1);
Adafruit_DCMotor *backRight = AFMS.getMotor(2);
Adafruit_DCMotor *frontLeft = AFMS.getMotor(3);
Adafruit_DCMotor *frontRight = AFMS.getMotor(4);

int incomingByte = 0;

enum Cmd {
  Forward,
  Backward,
  Release
};

void setup() {
  Serial.begin(9600);
  AFMS.begin();
}


void loop() {
  setSpeed(100);

  if (Serial.available() > 0) {
    cmd = Serial.readStringUntil(';');
    op = getValue(cmd, ',', 0);
    params = getValue(cmd, ',', 1);

    switch (op) {
      case 'F':
        moveForward(params);
        break;
      case 'B':
        moveBackward(params);
        break;
      case 'S':
        stop(params);
        break;
      case 'L':
        turnLeft(params);
        break;
      case 'R':
        turnRight(params);
        break;
      default:
        // DO NOTHING
        break;
    }
    Serial.print("Received: ");
    Serial.println(incomingByte, DEC);
  }



//    BLRun(Cmd::Release);
//    FLRun(Cmd::Release);
//
//    BRRun(Cmd::Forward);
//    FRRun(Cmd::Forward);
}

void setSpeed(int speed) {
  backLeft->setSpeed(speed);
  backRight->setSpeed(speed);
  frontLeft->setSpeed(speed);
  frontRight->setSpeed(speed);
}

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void runCmdForMotor(Adafruit_DCMotor *motor, Cmd cmd, int forward, int backward) {
   if (cmd == Cmd::Forward) {
    motor->run(forward);
  } else if (cmd == Cmd::Backward) {
    motor->run(backward);
  } else if (cmd == Cmd::Release) {
   motor->run(RELEASE);
  }
}

void BLRun(Cmd cmd) {
  runCmdForMotor(backLeft, cmd, FORWARD, BACKWARD);
}

void BRRun(Cmd cmd) {
  runCmdForMotor(backRight, cmd, BACKWARD, FORWARD);
}

void FLRun(Cmd cmd) {
  runCmdForMotor(frontLeft, cmd, FORWARD, BACKWARD);
}

void FRRun(Cmd cmd) {
  runCmdForMotor(frontRight, cmd, BACKWARD, FORWARD);
}
