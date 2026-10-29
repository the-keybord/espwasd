/****************************************************************
 *  ==============================================================
 *   RBFesp - ESP32 Remote Controller Core
 *  ==============================================================
 *   NOTE FOR STUDENTS:
 *   You DO NOT need to edit or touch this file!
 *   Edit your code in: ButtonHandler.h, p1.h, p2.h, p3.h, p4.h
 *  ==============================================================
 ****************************************************************/

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "RemoteCore.h"

void _sendRemotePacket(const String &text);
void _sendRemoteRaw(const String &packet);

RemoteMonitor monitor;
RemoteController remote;

#define BLUETOOTH_NAME "ESP32_RoboAndrian"
#define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
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

#include "ButtonHandler.h"
#include "p1.h"
#include "p2.h"
#include "p3.h"
#include "p4.h"

void executeProgram(char programKey) {
  isProgramRunning = true;
  Serial.print(">>> Program ");
  Serial.print(programKey);
  Serial.println(" STARTING <<<");

  _sendRemoteRaw("LOCK:P" + String(programKey));
  monitor.println(">>> RUNNING P",
                  String(programKey) + " (Interaction Paused) <<<");
  delay(50);

  if (programKey == '1')
    p1();
  else if (programKey == '2')
    p2();
  else if (programKey == '3')
    p3();
  else if (programKey == '4')
    p4();

  Serial.print(">>> Program ");
  Serial.print(programKey);
  Serial.println(" FINISHED <<<");

  monitor.println(">>> P", String(programKey) + " Done! Ready.");
  _sendRemoteRaw("UNLOCK");
  isProgramRunning = false;
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    isConnected = true;
    remote._setConnected(true);
    Serial.println(">>> RBFesp Connected! <<<");
    monitor.println("Connected! Ready.");
    _sendRemoteRaw("UNLOCK");
  }

  void onDisconnect(BLEServer *pServer) {
    isConnected = false;
    remote._setConnected(false);
    Serial.println("<<< RBFesp Disconnected! >>>");
    pServer->startAdvertising();
  }
};

class MyRxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    if (isProgramRunning)
      return;

    String message = pCharacteristic->getValue().c_str();
    if (message.length() >= 2) {
      char action = message[0];
      char key = message[1];
      bool isPressed = (action == '+');
      remote._setKeyState(key, isPressed);

      if (isPressed && (key >= '1' && key <= '4')) {
        executeProgram(key);
        return;
      }
      if (!isPressed && (key >= '1' && key <= '4'))
        return;

      KeyEvent event(key, isPressed);
      onButton(event);
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting RBFesp Remote Controller...");

  BLEDevice::init(BLUETOOTH_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pRxCharacteristic->setCallbacks(new MyRxCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("RBFesp Ready! Open index.html and press Connect.");
  userSetup();
}

void loop() { delay(10); }
