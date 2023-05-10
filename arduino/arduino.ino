// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include "message.h"
#include <AFMotor.h>
#include <SoftwareSerial.h>

AF_DCMotor motor_FWD_R(1);
AF_DCMotor motor_FWD_L(2);
AF_DCMotor motor_RWD_R(4);
AF_DCMotor motor_RWD_L(3);

//AF_DCMotor motors[4] = {motor_FWD_R, motor_FWD_L, motor_RWD_R, motor_RWD_L};

SoftwareSerial interboard(2, 10);
byte recv = 0;
uint32_t control = 0;
uint32_t last_control = 0;

// Ultrasound sensor
const int ping = 11;
const int echo = 13;

Message ctrl_msg = { 0 };

void bytes_to_msg(uint32_t bytes) {
  ctrl_msg.header = (bytes >> (8 * 3)) & 0xFF;
  ctrl_msg.cmd_ident = (bytes >> (8 * 2)) & 0xFF;
  ctrl_msg.payload = (bytes >> (8 * 1)) & 0xFF;
  ctrl_msg.end = bytes & 0xFF;
}

void setup() {
  Serial.begin(9600);     /* set up debugging serial communication */
  interboard.begin(9600); /* set up interboard uart communication */
  Serial.println("Motor test!");

  // turn on motor
  motor_FWD_R.setSpeed(255);
  motor_FWD_L.setSpeed(255);
  motor_RWD_R.setSpeed(255);
  motor_RWD_L.setSpeed(255);
  // inverted
  motor_FWD_R.run(RELEASE);
  motor_FWD_L.run(RELEASE);
  motor_RWD_R.run(RELEASE);
  // inverted
  motor_RWD_L.run(RELEASE);

  pinMode(ping, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  int i = 0;
  // check ultrasound
  analogWrite(ping, 127);
  long soundPulse = pulseIn(echo, HIGH);
  if (soundPulse != 0)
    Serial.println(soundPulse / 29 / 2);

  // Serial.print("tick");
  while (interboard.available() > 0) {
    if (i == 4) {
      i = 0;
      return;
    }
    recv = interboard.read();
    Serial.print("Byte");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(recv);
    Serial.print("\n");
    control |= (uint32_t)recv << (8 * i);
    i++;
  }
  bytes_to_msg(control);

  if (ctrl_msg.header == 0xAA) {
    Serial.println("Control");
    Serial.println(control);
    Serial.println(ctrl_msg.payload);

    if (ctrl_msg.cmd_ident == MOTOR) {
      // Some safety checks...
      if (((ctrl_msg.cmd_ident & FWD_R) && (ctrl_msg.cmd_ident & FWD_R_BACK)) || ((ctrl_msg.cmd_ident & FWD_L) && (ctrl_msg.cmd_ident & FWD_L_BACK)) || ((ctrl_msg.cmd_ident & RWD_R) && (ctrl_msg.cmd_ident & RWD_R_BACK)) || ((ctrl_msg.cmd_ident & RWD_L) && (ctrl_msg.cmd_ident & RWD_L_BACK))) {
        Serial.println(
          "Some wheel has been instructed to move backwards and forwards \
        at the same time");
        return;
      }
      Serial.println("Moving something");
      if (ctrl_msg.payload == 0) {
        Serial.println("Stopping");
        motor_FWD_L.run(RELEASE);
        motor_FWD_R.run(RELEASE);
        motor_RWD_R.run(RELEASE);
        motor_RWD_L.run(RELEASE);
      }
      // Forward movement
      if (ctrl_msg.payload & FWD_R) {
        Serial.println("FWD_R");
        motor_FWD_R.run(FORWARD);
      } else
        motor_FWD_R.run(BRAKE);

      if (ctrl_msg.payload & FWD_L) {
        Serial.println("FWD_L");
        motor_FWD_L.run(BACKWARD);
      } else
        motor_FWD_L.run(BRAKE);

      if (ctrl_msg.payload & RWD_R) {
        Serial.println("RWD_R");
        motor_RWD_R.run(FORWARD);
      } else
        motor_RWD_R.run(BRAKE);

      if (ctrl_msg.payload & RWD_L) {
        Serial.println("RWD_L");
        motor_RWD_L.run(BACKWARD);
      } else
        motor_RWD_L.run(BRAKE);

      // Backward movement
      if (ctrl_msg.payload & FWD_R_BACK) {
        Serial.println("FWD_R_BACK");
        motor_FWD_R.run(BACKWARD);
      }
      if (ctrl_msg.payload & FWD_L_BACK) {
        Serial.println("FWD_L_BACK");
        motor_FWD_L.run(FORWARD);
      }
      if (ctrl_msg.payload & RWD_R_BACK) {
        Serial.println("RWD_R_BACK");
        motor_RWD_R.run(BACKWARD);
      }
      if (ctrl_msg.payload & RWD_L_BACK) {
        Serial.println("RWD_L_BACK");
        motor_RWD_L.run(FORWARD);
      }
    }
    last_control = control;

  } else { /* Stop all wheels if no command has been received, to prevent */
           /* them from moving permanently */
    //for (int i = 0; i < 4; i++)
    //motors[i].run(FORWARD);
  }
  control = 0;

  delay(100);

  //motor_FWD_L.run(RELEASE);
  //motor_FWD_R.run(RELEASE);
  //motor_RWD_R.run(RELEASE);
  //motor_RWD_L.run(RELEASE);
}