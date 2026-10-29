#pragma once
#include "RemoteCore.h"

// ==============================================================
//  CTM - PROGRAM 4 (p4): Motors + Continuous Servo Hardware Test
// ==============================================================
void p4() {
  Serial.println(">>> CTM Program 4: Hardware Test <<<");

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

  // 2) Test Motor Left (27, 13)
  monitor.println("Test: Left Motor (27,13) Fwd");
  setPixel(0, COLOR_CYAN);
  setMotors(200, 0);
  delay(600);

  monitor.println("Test: Left Motor (27,13) Rev");
  setMotors(-200, 0);
  delay(600);

  stopMotors();
  clearPixels();
  delay(300);

  // 3) Test Motor Right (4, 2)
  monitor.println("Test: Right Motor (4,2) Fwd");
  setPixel(1, COLOR_CYAN);
  setMotors(0, 200);
  delay(600);

  monitor.println("Test: Right Motor (4,2) Rev");
  setMotors(0, -200);
  delay(600);

  stopMotors();
  clearPixels();
  delay(300);

  // 4) Test Continuous Servo (GPIO 32)
  monitor.println("Test: Servo (Pin 32) Forward");
  setPixel(2, COLOR_GREEN);
  servoForward(100);
  delay(600);

  monitor.println("Test: Servo (Pin 32) Backward");
  setPixel(3, COLOR_PURPLE);
  servoBackward(100);
  delay(600);

  servoStop();
  stopMotors();
  clearPixels();
  setAllPixels(COLOR_GREEN);
  monitor.println("CTM Hardware Test: All Passed!");
  delay(600);
  clearPixels();
}
