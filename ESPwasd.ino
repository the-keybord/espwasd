/****************************************************************
 *  ==============================================================
 *   ESP32 WASD Remote Controller - System Core Engine
 *   Configured for Maker ESP32 Robot
 *   Motors: M1 (27, 13), M2 (4, 2)
 *   LED Strip: 4 NeoPixels on GPIO 16
 *  ==============================================================
 *   NOTE FOR STUDENTS / KIDS:
 *   You DO NOT need to edit or touch this file!
 *   This file connects to Bluetooth and manages the system.
 *   
 *   Where to write your code:
 *   - Setup & Buttons: open the "ButtonHandler.h" tab
 *   - Programs 1 to 4: open "p1.h", "p2.h", "p3.h", "p4.h" tabs
 *  ==============================================================
 ****************************************************************/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>

#include "RemoteCore.h"

// Forward declaration of internal transmitter packet functions
void _sendRemotePacket(const String &text);
void _sendRemoteRaw(const String &packet);

// Global instances available throughout the project
RemoteMonitor monitor;
RemoteController remote;

// Initialize 4 NeoPixels on GPIO 16
Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Bluetooth Configuration
#define BLUETOOTH_NAME         "ESP32_WASD_PAD"
#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

// Bluetooth State
BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic = NULL;
bool isConnected = false;
bool isProgramRunning = false; // When true, remote screen interaction pauses

// Send raw BLE packet to remote
void _sendRemoteRaw(const String &packet) {
  if (isConnected && pTxCharacteristic != NULL) {
    pTxCharacteristic->setValue(packet.c_str());
    pTxCharacteristic->notify();
  }
}

// Internal packet sender for serial monitor stream
void _sendRemotePacket(const String &text) {
  Serial.print(text);
  _sendRemoteRaw("M:" + text);
}

// Include programs 1-4 and student button handler
#include "p1.h"
#include "p2.h"
#include "p3.h"
#include "p4.h"
#include "ButtonHandler.h"

// Hardcoded Program Runner: Pauses screen interaction until finished
void executeProgram(char programKey) {
  isProgramRunning = true;

  Serial.print(">>> Program ");
  Serial.print(programKey);
  Serial.println(" STARTING (Remote interaction paused) <<<");

  // Send lock packet to web page so it disables buttons and shows the overlay
  _sendRemoteRaw("LOCK:PROGRAM " + String(programKey));

  // Inform student on the remote monitor that interaction is paused
  monitor.println(">>> RUNNING P", String(programKey) + " (Interaction Paused) <<<");

  // Small delay so BLE packets have time to arrive and render on browser
  delay(50);

  // Stop any previous movement before running program
  stopMotors();

  // Execute the selected program synchronously
  if (programKey == '1') {
    p1();
  } else if (programKey == '2') {
    p2();
  } else if (programKey == '3') {
    p3();
  } else if (programKey == '4') {
    p4();
  }

  // Safety: Ensure motors stop when program ends
  stopMotors();

  Serial.print(">>> Program ");
  Serial.print(programKey);
  Serial.println(" FINISHED (Remote resumed) <<<");

  // Restore ready status on remote monitor
  monitor.println(">>> P", String(programKey) + " Done! Ready.");

  // Send unlock packet to web page to dismiss overlay and re-enable buttons
  _sendRemoteRaw("UNLOCK");

  isProgramRunning = false;
}

// Bluetooth Connection Callbacks
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    isConnected = true;
    remote._setConnected(true);
    stopMotors(); // Ensure robot is stationary on connect
    Serial.println(">>> Remote Connected! <<<");
    monitor.println("Connected! Ready to drive.");
    _sendRemoteRaw("UNLOCK");
  }

  void onDisconnect(BLEServer* pServer) {
    isConnected = false;
    remote._setConnected(false);
    stopMotors(); // CRITICAL SAFETY: Stop robot if connection drops!
    clearPixels();
    Serial.println("<<< Remote Disconnected! Motors stopped. >>>");
    pServer->startAdvertising();
  }
};

// Bluetooth Incoming Message Callbacks (Remote Buttons -> ESP32)
class MyRxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    // If a program is running, pause/ignore incoming button interactions
    if (isProgramRunning) {
      Serial.println("[Paused] Button ignored while program is running");
      return;
    }

    String message = pCharacteristic->getValue().c_str();

    if (message.length() >= 2) {
      char action = message[0]; // '+' for pressed, '-' for released
      char key    = message[1]; // The character, like 'W', 'A', '1', etc.

      bool isPressed = (action == '+');
      remote._setKeyState(key, isPressed);

      // Check if button 1, 2, 3, or 4 was pressed (Hardcoded program launcher)
      if (isPressed && (key == '1' || key == '2' || key == '3' || key == '4')) {
        executeProgram(key);
        return;
      }

      // Ignore release event for program keys 1-4
      if (!isPressed && (key == '1' || key == '2' || key == '3' || key == '4')) {
        return;
      }

      // For all other buttons, pass single event object to student's ButtonHandler
      KeyEvent event(key, isPressed);
      onButton(event);
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Maker ESP32 Robot Controller...");

  // Step 1: Initialize Motors (M1: 27, 13; M2: 4, 2)
  motorsInit();

  // Step 2: Initialize NeoPixels (Pin 16, 4 LEDs)
  pixels.begin();
  pixels.setBrightness(60); // Comfortable brightness for testing
  pixels.clear();
  pixels.show();

  // Step 3: Initialize Bluetooth
  BLEDevice::init(BLUETOOTH_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Setup TX Characteristic (ESP32 -> Web displays & lock events)
  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  // Setup RX Characteristic (Web buttons -> ESP32)
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  pRxCharacteristic->setCallbacks(new MyRxCallbacks());

  // Start Bluetooth advertising
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Ready! Open index.html and press Connect.");

  // Step 4: Call student's userSetup from ButtonHandler.h
  userSetup();
}

void loop() {
  // Bluetooth communication runs in the background
  delay(10);
}
