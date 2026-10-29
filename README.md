# ESPwasd

A modern, responsive Web Bluetooth remote controller and robotics framework for ESP32 and BBC micro:bit.

## Features

- **Web Bluetooth Remote Controller (`index.html`)**:
  - 4x4 interactive virtual keypad (1-4, Q-R, A-F, Z-V) with multi-touch and keyboard support.
  - Built-in live **Serial Monitor** streaming logs directly from firmware over BLE with auto-scroll.
  - Program execution locking & pause overlay while autonomous routines run.
- **ESP32 Firmware Modules**:
  - **`ESPwasd.ino`**: System Core Engine for Maker ESP32 Robot with motors (Left: 27, 13; Right: 4, 2) and NeoPixel strip (GPIO 16).
  - **`CTM/`**: ESP32 controller with dual H-bridge motors (Left: 27, 13; Right: 4, 2), NeoPixels, and dedicated hardware PWM for a 50Hz continuous rotation servo on **GPIO 32**.
  - **`RBFesp/`**: Lightweight modular core template.
- **`RBFmicro/`**: Python receiver script for BBC micro:bit BLE control.

## Getting Started

1. Open `index.html` in a Web Bluetooth capable browser (Chrome, Edge, Chromium on Android).
2. Power on your ESP32 or micro:bit.
3. Click **Connect** and pair with your device.
4. Drive using the on-screen buttons or WASD on your keyboard!
