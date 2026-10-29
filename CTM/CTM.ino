/****************************************************************
 *  ==============================================================
 *   CTM - ESP32 Motor Controller + Continuous Servo
 *   Motors: Left (27, 13), Right (4, 2)
 *   Continuous Servo: GPIO 32
 *   LED Strip: 4 NeoPixels on GPIO 16
 *  ==============================================================
 *   NOTE FOR STUDENTS:
 *   You DO NOT need to edit or touch this file!
 *   Edit your code in: ButtonHandler.h, p1.h, p2.h, p3.h, p4.h
 *  ==============================================================
 ****************************************************************/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>

#include "RemoteCore.h"

void _sendRemotePacket(const String &text);
void _sendRemoteRaw(const String &packet);

RemoteMonitor monitor;
RemoteController remote;

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define BLUETOOTH_NAME         "ESP32_CTM_L298N"
#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic = NULL;
bool isConnected = false;
bool isProgramRunning = false;

void _sendRemoteRaw(const String &packet) {
  if (isConnected && pTxCharacteristic != NULL) {
    pTxCharacteristic->setValue(packet.c_str());
    pTxCharacteristic->notify();
  }
}

void _sendRemotePacket(const String &text) {
  Serial.print(text);
  _sendRemoteRaw("M:" + text);
}

#include "p1.h"
#include "p2.h"
#include "p3.h"
#include "p4.h"
#include "ButtonHandler.h"

void executeProgram(char programKey) {
  isProgramRunning = true;
  Serial.print(">>> CTM Program ");
  Serial.print(programKey);
  Serial.println(" STARTING <<<");

  _sendRemoteRaw("LOCK:CTM P" + String(programKey));
  monitor.println(">>> RUNNING P", String(programKey) + " (Interaction Paused) <<<");
  delay(50);

  stopMotors();
  servoStop();

  if (programKey == '1') p1();
  else if (programKey == '2') p2();
  else if (programKey == '3') p3();
  else if (programKey == '4') p4();

  stopMotors();
  servoStop();

  Serial.print(">>> CTM Program ");
  Serial.print(programKey);
  Serial.println(" FINISHED <<<");

  monitor.println(">>> P", String(programKey) + " Done! Ready.");
  _sendRemoteRaw("UNLOCK");
  isProgramRunning = false;
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    isConnected = true;
    remote._setConnected(true);
    stopMotors();
    servoStop();
    Serial.println(">>> CTM Connected! <<<");
    monitor.println("CTM Connected! Motors (27,13 / 4,2) + Servo (32) Ready.");
    _sendRemoteRaw("UNLOCK");
  }

  void onDisconnect(BLEServer* pServer) {
    isConnected = false;
    remote._setConnected(false);
    stopMotors();
    servoStop();
    clearPixels();
    Serial.println("<<< CTM Disconnected! Motors & Servo stopped. >>>");
    pServer->startAdvertising();
  }
};

class MyRxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    if (isProgramRunning) return;

    String message = pCharacteristic->getValue().c_str();
    if (message.length() >= 2) {
      char action = message[0];
      char key    = message[1];
      bool isPressed = (action == '+');
      remote._setKeyState(key, isPressed);

      if (isPressed && (key >= '1' && key <= '4')) {
        executeProgram(key);
        return;
      }
      if (!isPressed && (key >= '1' && key <= '4')) return;

      KeyEvent event(key, isPressed);
      onButton(event);
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting CTM (ESP32 Motors: 27,13 / 4,2 + Servo: 32)...");

  motorsInit();
  servoInit();

  pixels.begin();
  pixels.setBrightness(60);
  pixels.clear();
  pixels.show();

  BLEDevice::init(BLUETOOTH_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  pRxCharacteristic->setCallbacks(new MyRxCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("CTM Ready! Open index.html and press Connect.");
  userSetup();
}

void loop() {
  delay(10);
}
