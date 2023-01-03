#pragma once

#include <Arduino.h>

#include "hid_project_include.h"

#include "rotary.h"
#include "usb.h"

class JoystickBind {
public:
  uint8_t id;
};

class Input {
protected:
  void log(const char *nameProgmem, const char *arg);

private:
  void _log(const char *nameProgmem, const char *arg);
};

// Const parameters for Switch2Pos. Stays in flash, saves RAM.
class Switch2PosConf {
public:
  const char *name; // using progmem!
  KeyBind key1;
  JoystickBind joy1;
};

class Switch2Pos : public Input {
public:
  void setup(Uplink &uplink, const Switch2PosConf *conf);
  void loop(Uplink &uplink, const Switch2PosConf *conf, uint8_t value);

private:
  uint8_t cooldown : 4;
  uint8_t stableState : 2;
  uint8_t lastState : 2;
};

// Const parameters for ButtonConf. Stays in flash, saves RAM.
class ButtonConf {
public:
  const char *name; // using progmem!
  KeyBind key1;
  JoystickBind joy1;
};

class Button : public Input {
public:
  void setup(Uplink &uplink, const ButtonConf *conf);
  void loop(Uplink &uplink, const ButtonConf *conf, uint8_t value);

private:
  uint8_t cooldown : 4;
  uint8_t stableState : 2;
  uint8_t lastState : 2;
};

// Const parameters for ButtonConf. Stays in flash, saves RAM.
class AdvButtonConf {
public:
  const char *name; // using progmem!
  KeyBind key1;     // Short press
  JoystickBind joy1;
  KeyBind key2; // Long press
};

// Like Button, but can send different key for short and long presses.
class AdvButton : public Input {
public:
  void setup(Uplink &uplink, const AdvButtonConf *conf);
  void loop(Uplink &uplink, const AdvButtonConf *conf, uint8_t value);

private:
  uint8_t cooldown : 4;
  uint8_t stableState : 1;
  uint8_t lastState : 1;
  uint8_t longPressDone : 1;
  unsigned long lastStableChangeMillis;
};

// Const parameters for Switch3Pos. Stays in flash, saves RAM.
class Switch3PosConf {
public:
  const char *name; // using progmem!
  KeyBind key1;
  KeyBind key2;
  KeyBind key3;
  JoystickBind joy1;
  JoystickBind joy2;
  JoystickBind joy3;
};

class Switch3Pos : public Input {
public:
  void setup(Uplink &uplink, const Switch3PosConf *conf);
  void loop(Uplink &uplink, const Switch3PosConf *conf, uint8_t value1,
            uint8_t value2);

private:
  uint8_t cooldown : 4;
  uint8_t stableState : 2;
  uint8_t lastState : 2;
};

// Const parameters for RotaryEncoderConf. Stays in flash, saves RAM.
class RotaryEncoderConf {
public:
  const char *name; // using progmem!
  KeyBind key1;
  KeyBind key2;
  JoystickBind joy1;
  JoystickBind joy2;
};

class RotaryEncoder : public Input {
public:
  void setup(Uplink &uplink, const RotaryEncoderConf *conf);
  void loop(Uplink &uplink, const RotaryEncoderConf *conf, uint8_t value1,
            uint8_t value2);

private:
  RotaryDecoder decoder;
  uint8_t needReset = false;
};