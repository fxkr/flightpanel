#include <Arduino.h>

#include <Joystick.h>

#include "usb.h"

void Uplink::loop() {
#ifndef DEBUG_DISABLE_UPLINK
  if (changed1) {
    joystick1.sendState();
    changed1 = false;
  }
  if (changed2) {
    joystick2.sendState();
    changed2 = false;
  }
  if (changed3) {
    joystick3.sendState();
    changed3 = false;
  }
#endif
}

void Uplink::setup() {
#ifndef DEBUG_DISABLE_UPLINK
  joystick1.begin();
  joystick2.begin();
  joystick3.begin();
  keyboard.begin();
#endif
}

void Uplink::setButton(uint8_t button, uint8_t value) {
#ifndef DEBUG_DISABLE_UPLINK
  // Zero is reserved for no action
  if (button == 0) {
    return;
  }

  // But on the USB joystick, the first button has ID zero
  button -= 1;

  // Distribute buttons to joysticks
  if (button < 80) { // 0..79: Joystick 1
    joystick1.setButton(button - 0, value);
    changed1 = true;
  } else if (button < 160) { // 80..159: Joystick 2
    joystick2.setButton(button - 80, value);
    changed2 = true;
  } else if (button < 240) { // 160..239: Joystick 3
    joystick3.setButton(button - 160, value);
    changed3 = true;
  }
#endif
}

void Uplink::pushButton(const KeyBind *kb) {
#ifndef DEBUG_DISABLE_UPLINK
  if (!kb->code) {
    return;
  }

  if (kb->lalt) {
    keyboard.add(KEY_LEFT_ALT);
  }
  if (kb->lctrl) {
    keyboard.add(KEY_LEFT_CTRL);
  }
  if (kb->lshift) {
    keyboard.add(KEY_LEFT_SHIFT);
  }
  if (kb->lwin) {
    keyboard.add(KEY_LEFT_WINDOWS);
  }
  if (kb->ralt) {
    keyboard.add(KEY_RIGHT_ALT);
  }
  if (kb->rctrl) {
    keyboard.add(KEY_RIGHT_CTRL);
  }
  if (kb->rshift) {
    keyboard.add(KEY_RIGHT_SHIFT);
  }
  if (kb->rwin) {
    keyboard.add(KEY_RIGHT_WINDOWS);
  }
  keyboard.add(kb->code);
  keyboard.send();

  keyboard.removeAll();
  keyboard.send();
#endif
}