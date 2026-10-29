#pragma once
#include <Arduino.h>

/****************************************************************
 *  RBFesp - RemoteCore.h
 *  --------------------------------------------------------------
 *  Clean, minimal core classes for ESP32 Remote Controller:
 *  - KeyEvent: passed to onButton() with key, pressed, released
 *  - RemoteMonitor: serial-style monitor (monitor.print / println)
 *  - RemoteController: tracks connection and held key states
 ****************************************************************/

// Forward declarations of internal transmitter packet functions (defined in RBFesp.ino)
void _sendRemotePacket(const String &text);
void _sendRemoteRaw(const String &packet);

// ==============================================================
//  KeyEvent Class
// ==============================================================
class KeyEvent {
public:
  char key;        // The button character ('1'-'4', 'Q'-'R', 'A'-'F', 'Z'-'V')
  bool pressed;    // true when button is pushed down
  bool released;   // true when button is released
  char action;     // '+' for press, '-' for release

  KeyEvent(char k = ' ', bool isPress = true) {
    key = k;
    pressed = isPress;
    released = !isPress;
    action = isPress ? '+' : '-';
  }

  // Implicit conversion to char (allows: if (event == 'A'))
  operator char() const { return key; }

  // Comparison operators for easy student writing
  bool operator==(char c) const { return key == c; }
  bool operator!=(char c) const { return key != c; }

  // Helper methods
  bool is(char k) const { return key == k; }
  bool isPressed(char k) const { return key == k && pressed; }
  bool isReleased(char k) const { return key == k && released; }
};

// ==============================================================
//  RemoteMonitor Class
//  Acts like a Serial Monitor over Bluetooth (monitor.print / println)
// ==============================================================
class RemoteMonitor {
public:
  RemoteMonitor() {}

  // print(...) - appends data to the stream without a newline
  void print(const String &text) {
    _sendRemotePacket(text);
  }

  void print(const char* text) {
    _sendRemotePacket(String(text));
  }

  void print(char c) {
    _sendRemotePacket(String(c));
  }

  void print(int val) {
    _sendRemotePacket(String(val));
  }

  void print(unsigned int val) {
    _sendRemotePacket(String(val));
  }

  void print(long val) {
    _sendRemotePacket(String(val));
  }

  void print(unsigned long val) {
    _sendRemotePacket(String(val));
  }

  void print(float val, int decimals = 2) {
    _sendRemotePacket(String(val, decimals));
  }

  void print(double val, int decimals = 2) {
    _sendRemotePacket(String(val, decimals));
  }

  void print(bool val) {
    _sendRemotePacket(val ? "1" : "0");
  }

  template <typename T1, typename T2>
  void print(const T1 &prefix, const T2 &value) {
    _sendRemotePacket(String(prefix) + String(value));
  }

  // println(...) - appends data to the stream followed by a newline (\n)
  void println(const String &text) {
    _sendRemotePacket(text + "\n");
  }

  void println(const char* text) {
    _sendRemotePacket(String(text) + "\n");
  }

  void println(char c) {
    _sendRemotePacket(String(c) + "\n");
  }

  void println(int val) {
    _sendRemotePacket(String(val) + "\n");
  }

  void println(unsigned int val) {
    _sendRemotePacket(String(val) + "\n");
  }

  void println(long val) {
    _sendRemotePacket(String(val) + "\n");
  }

  void println(unsigned long val) {
    _sendRemotePacket(String(val) + "\n");
  }

  void println(float val, int decimals = 2) {
    _sendRemotePacket(String(val, decimals) + "\n");
  }

  void println(double val, int decimals = 2) {
    _sendRemotePacket(String(val, decimals) + "\n");
  }

  void println(bool val) {
    _sendRemotePacket(String(val ? "1" : "0") + "\n");
  }

  void println() {
    _sendRemotePacket("\n");
  }

  template <typename T1, typename T2>
  void println(const T1 &prefix, const T2 &value) {
    _sendRemotePacket(String(prefix) + String(value) + "\n");
  }

  // Clear the remote monitor screen
  void clear() {
    _sendRemoteRaw("CLEAR");
  }

  RemoteMonitor& operator=(const String &text) { println(text); return *this; }
  RemoteMonitor& operator=(const char* text)   { println(text); return *this; }
  RemoteMonitor& operator=(int val)            { println(val); return *this; }
  RemoteMonitor& operator=(float val)          { println(val); return *this; }
};

// ==============================================================
//  RemoteController Class
//  Tracks overall remote status (connection, held keys)
// ==============================================================
class RemoteController {
private:
  bool _connected;
  bool _keyState[256];

public:
  RemoteController() : _connected(false) {
    for (int i = 0; i < 256; i++) {
      _keyState[i] = false;
    }
  }

  bool isConnected() const {
    return _connected;
  }

  bool isKeyDown(char key) const {
    unsigned char idx = (unsigned char)key;
    return _keyState[idx];
  }

  bool isPressed(char key) const {
    return isKeyDown(key);
  }

  void _setConnected(bool conn) {
    _connected = conn;
    if (!conn) {
      for (int i = 0; i < 256; i++) {
        _keyState[i] = false;
      }
    }
  }

  void _setKeyState(char key, bool pressed) {
    unsigned char idx = (unsigned char)key;
    _keyState[idx] = pressed;
  }
};

// Global objects available throughout the project
extern RemoteMonitor monitor;
extern RemoteController remote;
