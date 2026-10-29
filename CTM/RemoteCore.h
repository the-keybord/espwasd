#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/****************************************************************
 *  CTM - RemoteCore.h
 *  --------------------------------------------------------------
 *  Core classes and drivers for ESP32 + Continuous Servo:
 *  - Motors:
 *      Motor Left:  GPIO 27, 13
 *      Motor Right: GPIO 4, 2
 *  - Continuous Servo: GPIO 32 (50Hz PWM)
 *  - NeoPixels: 4 LEDs on GPIO 16
 *  - KeyEvent: passed to onButton() with key, pressed, released
 *  - RemoteMonitor: serial-style monitor (monitor.print / println)
 *  - RemoteController: tracks connection and held key states
 ****************************************************************/

// ==============================================================
//  MOTOR CONFIGURATION (ESP32 Dual H-Bridge)
//  Left Motor:  M1A (GPIO 27), M1B (GPIO 13)
//  Right Motor: M2A (GPIO 4),  M2B (GPIO 2)
// ==============================================================
#define MOTOR_PIN_M1A 27
#define MOTOR_PIN_M1B 13
#define MOTOR_PIN_M2A 4
#define MOTOR_PIN_M2B 2

// Dedicated fixed hardware LEDC channels (Channels 0-3 for motors, 4 for servo)
// Eliminates channel exhaustion and dynamic allocation leaks permanently!
#define MOTOR_CH_M1A  0
#define MOTOR_CH_M1B  1
#define MOTOR_CH_M2A  2
#define MOTOR_CH_M2B  3

inline void _motorPinWrite(int pin, int channel, int duty) {
  if (duty > 255) duty = 255;
  if (duty < 0) duty = 0;

  #if defined(ESP32)
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      ledcWrite(pin, duty);
    #else
      ledcWrite(channel, duty);
    #endif
  #else
    analogWrite(pin, duty);
  #endif
}

inline void setMotorPins(int pinA, int chA, int pinB, int chB, int speed) {
  if (speed > 255) speed = 255;
  if (speed < -255) speed = -255;

  if (speed > 0) {
    _motorPinWrite(pinA, chA, speed);
    _motorPinWrite(pinB, chB, 0);
  } else if (speed < 0) {
    _motorPinWrite(pinA, chA, 0);
    _motorPinWrite(pinB, chB, -speed);
  } else {
    _motorPinWrite(pinA, chA, 0);
    _motorPinWrite(pinB, chB, 0);
  }
}

// Drive left (M1) and right (M2) motors with speeds between -255 and 255
inline void setMotors(int leftSpeed, int rightSpeed) {
  setMotorPins(MOTOR_PIN_M1A, MOTOR_CH_M1A, MOTOR_PIN_M1B, MOTOR_CH_M1B, leftSpeed);
  setMotorPins(MOTOR_PIN_M2A, MOTOR_CH_M2A, MOTOR_PIN_M2B, MOTOR_CH_M2B, rightSpeed);
}

// Emergency stop for both motors
inline void stopMotors() {
  _motorPinWrite(MOTOR_PIN_M1A, MOTOR_CH_M1A, 0);
  _motorPinWrite(MOTOR_PIN_M1B, MOTOR_CH_M1B, 0);
  _motorPinWrite(MOTOR_PIN_M2A, MOTOR_CH_M2A, 0);
  _motorPinWrite(MOTOR_PIN_M2B, MOTOR_CH_M2B, 0);
}

// Initialize motor GPIO pins with fixed 1kHz 8-bit PWM channels
inline void motorsInit() {
  #if defined(ESP32)
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      ledcAttachChannel(MOTOR_PIN_M1A, 1000, 8, MOTOR_CH_M1A);
      ledcAttachChannel(MOTOR_PIN_M1B, 1000, 8, MOTOR_CH_M1B);
      ledcAttachChannel(MOTOR_PIN_M2A, 1000, 8, MOTOR_CH_M2A);
      ledcAttachChannel(MOTOR_PIN_M2B, 1000, 8, MOTOR_CH_M2B);
    #else
      ledcSetup(MOTOR_CH_M1A, 1000, 8);
      ledcAttachPin(MOTOR_PIN_M1A, MOTOR_CH_M1A);

      ledcSetup(MOTOR_CH_M1B, 1000, 8);
      ledcAttachPin(MOTOR_PIN_M1B, MOTOR_CH_M1B);

      ledcSetup(MOTOR_CH_M2A, 1000, 8);
      ledcAttachPin(MOTOR_PIN_M2A, MOTOR_CH_M2A);

      ledcSetup(MOTOR_CH_M2B, 1000, 8);
      ledcAttachPin(MOTOR_PIN_M2B, MOTOR_CH_M2B);
    #endif
  #else
    pinMode(MOTOR_PIN_M1A, OUTPUT);
    pinMode(MOTOR_PIN_M1B, OUTPUT);
    pinMode(MOTOR_PIN_M2A, OUTPUT);
    pinMode(MOTOR_PIN_M2B, OUTPUT);
  #endif
  stopMotors();
}

// High-level student-friendly movement functions
inline void moveForward(int speed = 255) {
  setMotors(speed, speed);
}

inline void moveBackward(int speed = 255) {
  setMotors(-speed, -speed);
}

inline void turnLeft(int speed = 255) {
  setMotors(-speed, speed); // Spin turn in place
}

inline void turnRight(int speed = 255) {
  setMotors(speed, -speed); // Spin turn in place
}

inline void pivotLeft(int speed = 255) {
  setMotors(0, speed);      // Gentle pivot around left wheel
}

inline void pivotRight(int speed = 255) {
  setMotors(speed, 0);      // Gentle pivot around right wheel
}

// ==============================================================
//  CONTINUOUS SERVO CONFIGURATION (GPIO 32)
// ==============================================================
#define SERVO_PIN          32
#define SERVO_LEDC_CHANNEL 4
#define SERVO_FREQ_HZ      50
#define SERVO_RES_BITS     14
#define SERVO_US_MIN       1000 // Full speed Counter-Clockwise / Backward
#define SERVO_US_STOP      1500 // Neutral / Stop pulse (1.5 ms)
#define SERVO_US_MAX       2000 // Full speed Clockwise / Forward

inline void _servoWriteDuty(uint32_t duty) {
  #if defined(ESP32)
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      ledcWrite(SERVO_PIN, duty);
    #else
      ledcWrite(SERVO_LEDC_CHANNEL, duty);
    #endif
  #endif
}

// Write exact pulse width in microseconds (1000 to 2000 us, or 0 to disable pulse)
inline void servoWriteMicroseconds(int pulseUs) {
  if (pulseUs <= 0) {
    _servoWriteDuty(0);
    return;
  }
  if (pulseUs < 500)  pulseUs = 500;
  if (pulseUs > 2500) pulseUs = 2500;

  // 50Hz period = 20,000 us; 14-bit resolution = 16384 ticks
  uint32_t duty = ((uint32_t)pulseUs * (1 << SERVO_RES_BITS)) / 20000;
  _servoWriteDuty(duty);
}

// Stop continuous servo immediately (disables pulse to prevent neutral creep)
inline void servoStop() {
  _servoWriteDuty(0);
}

// Send active 1500us neutral pulse to servo
inline void servoNeutral() {
  servoWriteMicroseconds(SERVO_US_STOP);
}

// Set continuous servo speed from -100 (full reverse/CCW) to +100 (full forward/CW), 0 = stop
inline void setServo(int speed) {
  if (speed == 0) {
    servoStop();
    return;
  }
  if (speed > 100)  speed = 100;
  if (speed < -100) speed = -100;

  int pulseUs = SERVO_US_STOP + (speed * 5); // Maps -100..+100 -> 1000..2000 us
  servoWriteMicroseconds(pulseUs);
}

// Spin continuous servo forward / clockwise (speed 1 to 100)
inline void servoForward(int speed = 100) {
  if (speed < 0) speed = -speed;
  setServo(speed);
}

// Spin continuous servo backward / counter-clockwise (speed 1 to 100)
inline void servoBackward(int speed = 100) {
  if (speed < 0) speed = -speed;
  setServo(-speed);
}

inline void servoCW(int speed = 100)  { servoForward(speed); }
inline void servoCCW(int speed = 100) { servoBackward(speed); }

// Initialize continuous servo on GPIO 32 at 50Hz
inline void servoInit() {
  #if defined(ESP32)
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      ledcAttachChannel(SERVO_PIN, SERVO_FREQ_HZ, SERVO_RES_BITS, SERVO_LEDC_CHANNEL);
    #else
      ledcSetup(SERVO_LEDC_CHANNEL, SERVO_FREQ_HZ, SERVO_RES_BITS);
      ledcAttachPin(SERVO_PIN, SERVO_LEDC_CHANNEL);
    #endif
  #else
    pinMode(SERVO_PIN, OUTPUT);
  #endif
  servoStop();
}

// ==============================================================
//  NEOPIXEL CONFIGURATION (4 LEDs on GPIO 16)
// ==============================================================
#define PIXEL_PIN   16
#define NUM_PIXELS  4

extern Adafruit_NeoPixel pixels;

#define COLOR_OFF    pixels.Color(0, 0, 0)
#define COLOR_RED    pixels.Color(255, 0, 0)
#define COLOR_GREEN  pixels.Color(0, 255, 0)
#define COLOR_BLUE   pixels.Color(0, 0, 255)
#define COLOR_YELLOW pixels.Color(255, 180, 0)
#define COLOR_CYAN   pixels.Color(0, 255, 255)
#define COLOR_PURPLE pixels.Color(180, 0, 255)
#define COLOR_WHITE  pixels.Color(255, 255, 255)

inline void setPixel(int index, uint32_t color) {
  if (index >= 0 && index < NUM_PIXELS) {
    pixels.setPixelColor(index, color);
    pixels.show();
  }
}

inline void setAllPixels(uint32_t color) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

inline void clearPixels() {
  pixels.clear();
  pixels.show();
}

// Forward declarations of internal transmitter packet functions
void _sendRemotePacket(const String &text);
void _sendRemoteRaw(const String &packet);

// ==============================================================
//  KeyEvent Class
// ==============================================================
class KeyEvent {
public:
  char key;
  bool pressed;
  bool released;
  char action;

  KeyEvent(char k = ' ', bool isPress = true) {
    key = k;
    pressed = isPress;
    released = !isPress;
    action = isPress ? '+' : '-';
  }

  operator char() const { return key; }
  bool operator==(char c) const { return key == c; }
  bool operator!=(char c) const { return key != c; }

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
  void print(const String &text)      { _sendRemotePacket(text); }
  void print(const char* text)        { _sendRemotePacket(String(text)); }
  void print(char c)                  { _sendRemotePacket(String(c)); }
  void print(int val)                 { _sendRemotePacket(String(val)); }
  void print(unsigned int val)        { _sendRemotePacket(String(val)); }
  void print(long val)                { _sendRemotePacket(String(val)); }
  void print(unsigned long val)       { _sendRemotePacket(String(val)); }
  void print(float val, int dec = 2)  { _sendRemotePacket(String(val, dec)); }
  void print(double val, int dec = 2) { _sendRemotePacket(String(val, dec)); }
  void print(bool val)                { _sendRemotePacket(val ? "1" : "0"); }

  template <typename T1, typename T2>
  void print(const T1 &prefix, const T2 &value) {
    _sendRemotePacket(String(prefix) + String(value));
  }

  // println(...) - appends data to the stream followed by a newline (\n)
  void println(const String &text)      { _sendRemotePacket(text + "\n"); }
  void println(const char* text)        { _sendRemotePacket(String(text) + "\n"); }
  void println(char c)                  { _sendRemotePacket(String(c) + "\n"); }
  void println(int val)                 { _sendRemotePacket(String(val) + "\n"); }
  void println(unsigned int val)        { _sendRemotePacket(String(val) + "\n"); }
  void println(long val)                { _sendRemotePacket(String(val) + "\n"); }
  void println(unsigned long val)       { _sendRemotePacket(String(val) + "\n"); }
  void println(float val, int dec = 2)  { _sendRemotePacket(String(val, dec) + "\n"); }
  void println(double val, int dec = 2) { _sendRemotePacket(String(val, dec) + "\n"); }
  void println(bool val)                { _sendRemotePacket(String(val ? "1" : "0") + "\n"); }
  void println()                        { _sendRemotePacket("\n"); }

  template <typename T1, typename T2>
  void println(const T1 &prefix, const T2 &value) {
    _sendRemotePacket(String(prefix) + String(value) + "\n");
  }

  // Clear the remote monitor screen
  void clear() { _sendRemoteRaw("CLEAR"); }

  RemoteMonitor& operator=(const String &text) { println(text); return *this; }
  RemoteMonitor& operator=(const char* text)   { println(text); return *this; }
  RemoteMonitor& operator=(int val)            { println(val); return *this; }
  RemoteMonitor& operator=(float val)          { println(val); return *this; }
};

// ==============================================================
//  RemoteController Class
// ==============================================================
class RemoteController {
private:
  bool _connected;
  bool _keyState[256];

public:
  RemoteController() : _connected(false) {
    for (int i = 0; i < 256; i++) _keyState[i] = false;
  }

  bool isConnected() const { return _connected; }
  bool isKeyDown(char key) const { return _keyState[(unsigned char)key]; }
  bool isPressed(char key) const { return isKeyDown(key); }

  void _setConnected(bool conn) {
    _connected = conn;
    if (!conn) {
      for (int i = 0; i < 256; i++) _keyState[i] = false;
    }
  }

  void _setKeyState(char key, bool pressed) {
    _keyState[(unsigned char)key] = pressed;
  }
};

extern RemoteMonitor monitor;
extern RemoteController remote;
