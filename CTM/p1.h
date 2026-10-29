#pragma once
#include "RemoteCore.h"

// ==============================================================
//  CTM - PROGRAM 1 (p1): Victory Dance (L298N)
// ==============================================================
void p1() {
  Serial.println(">>> CTM Program 1: Victory Dance <<<");
  monitor.println("P1: Victory Dance - Dancing...");

  setPixel(0, COLOR_CYAN);
  setPixel(1, COLOR_CYAN);
  moveForward(220);
  delay(400);

  setAllPixels(COLOR_PURPLE);
  turnLeft(255);
  delay(700);

  clearPixels();
  setPixel(2, COLOR_RED);
  setPixel(3, COLOR_RED);
  moveBackward(220);
  delay(400);

  setAllPixels(COLOR_YELLOW);
  turnRight(255);
  delay(700);

  stopMotors();
  for (int i = 0; i < 3; i++) {
    setAllPixels(COLOR_GREEN);
    delay(150);
    clearPixels();
    delay(150);
  }
}
