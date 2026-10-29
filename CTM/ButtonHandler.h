#pragma once
#include "RemoteCore.h"

// ==============================================================
//  CTM - BUTTON HANDLER (Motors: 27,13 / 4,2 + Continuous Servo: Pin 32)
//  --------------------------------------------------------------
//  This runs whenever any button is pressed or released on the remote.
//
//  Available Continuous Servo Functions (GPIO 32):
//    servoForward();       // or servoForward(80); speed 1-100 (CW)
//    servoBackward();      // or servoBackward(80); speed 1-100 (CCW)
//    setServo(speed);      // -100 (full reverse) to +100 (full forward), 0 = stop
//    servoStop();          // stop continuous servo immediately!
// ==============================================================

void userSetup() {
  Serial.println("CTM Robot Ready (Motors: 27,13 / 4,2 + Servo: 32)!");
  // Motor pins: Left (27, 13), Right (4, 2)
  // Continuous Servo pin: GPIO 32
}

void onButton(KeyEvent event) {
  // 1) WASD Driving Controls
  if (event == 'W') {
    if (event.pressed) {
      monitor.println("[W] FORWARD");
      moveForward(255);
      setPixel(0, COLOR_CYAN);
      setPixel(1, COLOR_CYAN);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[W] STOPPED");
    }
  }
  else if (event == 'S') {
    if (event.pressed) {
      monitor.println("[S] BACKWARD");
      moveBackward(255);
      setPixel(2, COLOR_RED);
      setPixel(3, COLOR_RED);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[S] STOPPED");
    }
  }
  else if (event == 'A') {
    if (event.pressed) {
      monitor.println("[A] TURN LEFT");
      turnLeft(220);
      setPixel(0, COLOR_YELLOW);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[A] STOPPED");
    }
  }
  else if (event == 'D') {
    if (event.pressed) {
      monitor.println("[D] TURN RIGHT");
      turnRight(220);
      setPixel(1, COLOR_YELLOW);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[D] STOPPED");
    }
  }

  // 2) Continuous Servo Controls on Pin 32 (R = Forward/CW, F = Backward/CCW)
  else if (event == 'R') {
    if (event.pressed) {
      monitor.println("[R] SERVO FORWARD (CW)");
      servoForward(50);
      setPixel(0, COLOR_GREEN);
      setPixel(1, COLOR_GREEN);
    } else {
      servoStop();
      clearPixels();
      monitor.println("[R] SERVO STOPPED");
    }
  }
  else if (event == 'F') {
    if (event.pressed) {
      monitor.println("[F] SERVO BACKWARD (CCW)");
      servoBackward(50);
      setPixel(2, COLOR_PURPLE);
      setPixel(3, COLOR_PURPLE);
    } else {
      servoStop();
      clearPixels();
      monitor.println("[F] SERVO STOPPED");
    }
  }

  // 3) Extra Driving & Light Controls (Q, E, Z, X, C, V)
  else if (event == 'Q') {
    if (event.pressed) {
      monitor.println("[Q] PIVOT LEFT");
      pivotLeft(220);
      setPixel(0, COLOR_GREEN);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[Q] STOPPED");
    }
  }
  else if (event == 'E') {
    if (event.pressed) {
      monitor.println("[E] PIVOT RIGHT");
      pivotRight(220);
      setPixel(1, COLOR_GREEN);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[E] STOPPED");
    }
  }
  else if (event == 'X') {
    stopMotors();
    servoStop();
    setAllPixels(COLOR_RED);
    if (event.pressed) {
      monitor.println("[X] EMERGENCY STOP (Motors + Servo)");
    } else {
      clearPixels();
      monitor.println("[X] STOPPED");
    }
  }
  else if (event == 'Z') {
    if (event.pressed) {
      monitor.println("[Z] SPIN LEFT");
      turnLeft(255);
      setPixel(0, COLOR_PURPLE);
      setPixel(3, COLOR_PURPLE);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[Z] STOPPED");
    }
  }
  else if (event == 'C') {
    if (event.pressed) {
      monitor.println("[C] SPIN RIGHT");
      turnRight(255);
      setPixel(1, COLOR_PURPLE);
      setPixel(2, COLOR_PURPLE);
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[C] STOPPED");
    }
  }
  else if (event == 'V') {
    if (event.pressed) {
      monitor.println("[V] HAZARDS ON");
      setAllPixels(COLOR_YELLOW);
    } else {
      clearPixels();
      monitor.println("[V] READY");
    }
  }
}
