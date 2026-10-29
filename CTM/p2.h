#pragma once
#include "RemoteCore.h"

// ==============================================================
//  CTM - PROGRAM 2 (p2): Patrol Square (L298N)
// ==============================================================
void p2() {
  Serial.println(">>> CTM Program 2: Patrol Square <<<");
  monitor.println("P2: Patrol Square");

  for (int side = 1; side <= 4; side++) {
    monitor.println("Side ", side);

    clearPixels();
    setPixel(0, COLOR_CYAN);
    setPixel(1, COLOR_CYAN);
    moveForward(210);
    delay(700);

    stopMotors();
    delay(200);

    clearPixels();
    setPixel(1, COLOR_YELLOW);
    turnRight(230);
    delay(450);

    stopMotors();
    delay(200);
  }

  stopMotors();
  clearPixels();
  setAllPixels(COLOR_GREEN);
  delay(500);
  clearPixels();
}
