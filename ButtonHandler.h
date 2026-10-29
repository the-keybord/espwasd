#pragma once
#include "RemoteCore.h"

// ==============================================================
//  USER SETUP
//  --------------------------------------------------------------
//  This runs ONCE when the ESP32 starts up!
//  Kids can configure additional pins, sensors, or initial settings.
// ==============================================================
void userSetup() {
  Serial.println("Maker ESP32 Robot Ready!");

  // Motors (M1: 27, 13; M2: 4, 2) and NeoPixels (16)
  // are already initialized automatically!
}

// ==============================================================
//  BUTTON HANDLER
//  --------------------------------------------------------------
//  This function is called automatically every time ANY custom button
//  on the remote is pressed down or released.
//
//  Available Motor Functions:
//    moveForward();        // or moveForward(200); for speed 0-255
//    moveBackward();       // or moveBackward(200);
//    turnLeft();           // spin turn left
//    turnRight();          // spin turn right
//    pivotLeft();          // pivot around left wheel
//    pivotRight();         // pivot around right wheel
//    stopMotors();         // stop immediately!
//
//  Available NeoPixel Functions (Pin 16, 4 LEDs):
//    setPixel(0, COLOR_CYAN);
//    setAllPixels(COLOR_WHITE);
//    clearPixels();
// ==============================================================
void onButton(KeyEvent event) {

  // ------------------------------------------------------------
  // 1) WASD ROBOT MOVEMENT CONTROLS
  // ------------------------------------------------------------

  if (event == 'W') {
    if (event.pressed) {
      monitor.println("[W] FORWARD");
      moveForward(255);
      setPixel(0, COLOR_CYAN); // Headlight 1
      setPixel(1, COLOR_CYAN); // Headlight 2
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
      setPixel(2, COLOR_RED);  // Brake light 1
      setPixel(3, COLOR_RED);  // Brake light 2
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
      setPixel(0, COLOR_YELLOW); // Left blinker
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
      setPixel(1, COLOR_YELLOW); // Right blinker
    } else {
      stopMotors();
      clearPixels();
      monitor.println("[D] STOPPED");
    }
  }

  // ------------------------------------------------------------
  // 2) EXTRA CONTROLS (Q, E, R, F, Z, X, C, V)
  // ------------------------------------------------------------

  // Q & E: Gentle Pivot Turns
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

  // X: Emergency Brake / Stop
  else if (event == 'X') {
    stopMotors();
    setAllPixels(COLOR_RED);
    if (event.pressed) {
      monitor.println("[X] BRAKE!");
    } else {
      clearPixels();
      monitor.println("[X] STOPPED");
    }
  }

  // R: Police Flasher Effect
  else if (event == 'R') {
    if (event.pressed) {
      monitor.println("[R] POLICE SIREN");
      setPixel(0, COLOR_RED);
      setPixel(1, COLOR_RED);
      setPixel(2, COLOR_BLUE);
      setPixel(3, COLOR_BLUE);
    } else {
      clearPixels();
      monitor.println("[R] READY");
    }
  }

  // F: Front Headlights Toggle (High Beams)
  else if (event == 'F') {
    if (event.pressed) {
      monitor.println("[F] HIGH BEAMS");
      setAllPixels(COLOR_WHITE);
    } else {
      clearPixels();
      monitor.println("[F] READY");
    }
  }

  // Z: Fast 360 Spin Left
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

  // C: Fast 360 Spin Right
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

  // V: Hazard Warning Blinkers
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
