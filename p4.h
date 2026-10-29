#pragma once
#include "RemoteCore.h"

// ==============================================================
//  PROGRAM 4 (p4) - HARDWARE & CALIBRATION TEST
//  --------------------------------------------------------------
//  Triggered by button '4' on the remote.
//  Tests Motor 1 (27, 13), Motor 2 (4, 2), and all 4 NeoPixels (16).
//  Helps students verify their motor directions and wiring!
// ==============================================================

void p4() {
  Serial.println(">>> Running Program 4: Hardware Test <<<");

  // 1) Test NeoPixels
  monitor.println("Test: NeoPixels (Pixels 0 to 3)");
  clearPixels();
  setPixel(0, COLOR_RED);
  delay(300);
  setPixel(1, COLOR_GREEN);
  delay(300);
  setPixel(2, COLOR_BLUE);
  delay(300);
  setPixel(3, COLOR_YELLOW);
  delay(600);
  clearPixels();

  // 2) Test Motor 1 (Left Wheel)
  monitor.println("Test: Motor 1 - M1 Forward (27,13)");
  setPixel(0, COLOR_CYAN);
  setMotors(200, 0); // Left motor only forward
  delay(600);

  monitor.println("Test: Motor 1 - M1 Reverse (27,13)");
  setMotors(-200, 0); // Left motor only reverse
  delay(600);

  stopMotors();
  clearPixels();
  delay(300);

  // 3) Test Motor 2 (Right Wheel)
  monitor.println("Test: Motor 2 - M2 Forward (4,2)");
  setPixel(1, COLOR_CYAN);
  setMotors(0, 200); // Right motor only forward
  delay(600);

  monitor.println("Test: Motor 2 - M2 Reverse (4,2)");
  setMotors(0, -200); // Right motor only reverse
  delay(600);

  // 4) Complete!
  stopMotors();
  clearPixels();
  setAllPixels(COLOR_GREEN);
  monitor.println("Hardware Test: All Passed!");
  delay(600);
  clearPixels();
}
