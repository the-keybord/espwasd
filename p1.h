#pragma once
#include "RemoteCore.h"

// ==============================================================
//  PROGRAM 1 (p1) - ROBOT VICTORY DANCE
//  --------------------------------------------------------------
//  Triggered by button '1' on the remote.
//  The remote pauses while the robot dances!
// ==============================================================

void p1() {
  Serial.println(">>> Running Program 1: Victory Dance <<<");

  monitor.println("P1: Victory Dance - Dancing...");

  // Step 1: Step forward with cyan lights
  setPixel(0, COLOR_CYAN);
  setPixel(1, COLOR_CYAN);
  moveForward(220);
  delay(400);

  // Step 2: Spin 360 left with purple lights
  setAllPixels(COLOR_PURPLE);
  turnLeft(255);
  delay(700);

  // Step 3: Step backward with red brake lights
  clearPixels();
  setPixel(2, COLOR_RED);
  setPixel(3, COLOR_RED);
  moveBackward(220);
  delay(400);

  // Step 4: Spin 360 right with yellow lights
  setAllPixels(COLOR_YELLOW);
  turnRight(255);
  delay(700);

  // Step 5: Stop motors & victory light flash
  stopMotors();
  for (int i = 0; i < 3; i++) {
    setAllPixels(COLOR_GREEN);
    delay(150);
    clearPixels();
    delay(150);
  }
}
