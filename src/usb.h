#pragma once

#include "hid_project_include.h"

#include <Joystick.h>

#include <Arduino.h>

class KeyBind {
public:
  KeyboardKeycode code;
  uint8_t lwin : 1;
  uint8_t lctrl : 1;
  uint8_t lshift : 1;
  uint8_t lalt : 1;
  uint8_t rwin : 1;
  uint8_t rctrl : 1;
  uint8_t rshift : 1;
  uint8_t ralt : 1;
};

class Uplink {
private:
  bool changed1 = true;
  bool changed2 = true;
  bool changed3 = true;

  Joystick_ joystick1;
  Joystick_ joystick2;
  Joystick_ joystick3;
  SingleNKROKeyboard_ keyboard;

public:
  // See documentation why it is split like this. It's complicated.
  // If you need more buttons, increase the number of joysticks, _NOT_ the
  // number of buttons per joystick!
  Uplink()
      : joystick1(0x07, JOYSTICK_TYPE_JOYSTICK, 80, 0, false, false, false,
                  false, false, false, false, false, false, false, false),
        joystick2(0x08, JOYSTICK_TYPE_JOYSTICK, 80, 0, false, false, false,
                  false, false, false, false, false, false, false, false),
        joystick3(0x09, JOYSTICK_TYPE_JOYSTICK, 80, 0, false, false, false,
                  false, false, false, false, false, false, false, false){};

  void setButton(uint8_t button, uint8_t value);

  void pushButton(const KeyBind *kb);

  void setup();

  void loop();
};