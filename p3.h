#pragma once
#include "RemoteCore.h"

// ==============================================================
//  PROGRAM 3 (p3) - ENGINE REV & DRIFT SPIN
//  --------------------------------------------------------------
//  Triggered by button '3' on the remote.
//  Quick forward/backward pulses like an engine revving,
//  followed by a fast drift spin!
// ==============================================================

void p3() {
  Serial.println(">>> Running Program 3: Rev & Spin <<<");

  monitor.println("P3: Rev & Spin - Revving Up!");

  // 3 Rev bursts
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

  // Full blast 360 spin with rainbow chase
  monitor.println("Drift Spin!");
  turnLeft(255);
  for (int i = 0; i < 4; i++) {
    setPixel(i, COLOR_CYAN);
    delay(150);
  }
  delay(400);

  // Stop & clear
  stopMotors();
  clearPixels();
}
