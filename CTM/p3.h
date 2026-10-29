#pragma once
#include "RemoteCore.h"

// ==============================================================
//  CTM - PROGRAM 3 (p3): Rev & Drift Spin (L298N)
// ==============================================================
void p3() {
  Serial.println(">>> CTM Program 3: Rev & Spin <<<");
  monitor.println("P3: Rev & Spin - Revving Up!");

  for (int i = 0; i < 3; i++) {
    setAllPixels(COLOR_YELLOW);
    moveForward(255);
    delay(120);

    setAllPixels(COLOR_RED);
    moveBackward(255);
    delay(120);

    stopMotors();
    clearPixels();
    delay(150);
  }

  monitor.println("Drift Spin!");
  turnLeft(255);
  for (int i = 0; i < 4; i++) {
    setPixel(i, COLOR_CYAN);
    delay(150);
  }
  delay(400);

  stopMotors();
  clearPixels();
}
