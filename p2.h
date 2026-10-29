#pragma once
#include "RemoteCore.h"

// ==============================================================
//  PROGRAM 2 (p2) - PATROL SQUARE
//  --------------------------------------------------------------
//  Triggered by button '2' on the remote.
//  Drives in a square: forward, turn right, repeat 4 times!
// ==============================================================

void p2() {
  Serial.println(">>> Running Program 2: Patrol Square <<<");

  monitor.println("P2: Patrol Square");

  for (int side = 1; side <= 4; side++) {
    monitor.println("Side ", side);

    // Forward drive with headlights
    clearPixels();
    setPixel(0, COLOR_CYAN);
    setPixel(1, COLOR_CYAN);
    moveForward(210);
    delay(700);

    // Stop momentarily
    stopMotors();
    delay(200);

    // 90-degree right turn with right blinker
    clearPixels();
    setPixel(1, COLOR_YELLOW);
    turnRight(230);
    delay(450);

    // Stop before next side
    stopMotors();
    delay(200);
  }

  // Done!
  stopMotors();
  clearPixels();
  setAllPixels(COLOR_GREEN);
  delay(500);
  clearPixels();
}
