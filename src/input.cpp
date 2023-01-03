#include "rotary.h"

#include "input.h"

void Input::log(const char *nameProgmem, const char *arg) {
#if DEBUG_ENABLE_INPUT_LOG
  _log(nameProgmem, arg);
#endif
}

void Input::_log(const char *nameProgmem, const char *arg) {
  char name[32];
  strlcpy_P(name, nameProgmem, sizeof(name));

  if (name && arg && *name && *arg) {
    Serial.print(name);
    Serial.print(" ");
    Serial.print(arg);
    Serial.print("\r\n");
  }
}

void Button::setup(Uplink &uplink, const ButtonConf *conf) {
  uplink.setButton(conf->joy1.id, 0);
}

void Button::loop(Uplink &uplink, const ButtonConf *conf, uint8_t value) {
  // When not in cooldown (yet), new state can take effect
  if (!cooldown) {
    if (stableState != value) {
      stableState = value;
      log(conf->name, value ? "1" : "0");
      uplink.setButton(conf->joy1.id, value);
      uplink.pushButton(&conf->key1);
    }
  }

  // Cooldown expires only when state idle
  if (lastState == value) {
    if (cooldown) {
      cooldown--;
    }
  } else {
    // Note we use only 4 bit for it, so max is 16.
    cooldown = 3;
  }
  lastState = value;
}

void AdvButton::setup(Uplink &uplink, const AdvButtonConf *conf) {
  uplink.setButton(conf->joy1.id, 0);
}

void AdvButton::loop(Uplink &uplink, const AdvButtonConf *conf, uint8_t value) {
  const unsigned long shortPressLongPressThresholdMsec = 350;

  // When not in cooldown (yet), new state can take effect
  if (!cooldown) {
    if (stableState != value) {
      stableState = value;
      log(conf->name, value ? "1" : "0");

      uplink.setButton(conf->joy1.id, value);

      // On release, if no long push has been detected yet, it must've been
      // short
      if (!value && !longPressDone) {
        uplink.pushButton(&conf->key1);
      }

      lastStableChangeMillis = millis();
      longPressDone = false;
    } else {
      // Long button push?
      if (value && conf->key2.code && !longPressDone &&
          millis() - lastStableChangeMillis >
              shortPressLongPressThresholdMsec) {
        longPressDone = true;
        uplink.pushButton(&conf->key2);
      }
    }
  }

  // Cooldown expires only when state idle
  if (lastState == value) {
    if (cooldown) {
      cooldown--;
    }
  } else {
    // Note we use only 4 bit for it, so max is 16.
    cooldown = 3;
  }
  lastState = value;
}
void Switch2Pos::setup(Uplink &uplink, const Switch2PosConf *conf) {
  uplink.setButton(conf->joy1.id, 0);
}

void Switch2Pos::loop(Uplink &uplink, const Switch2PosConf *conf,
                      uint8_t value) {
  // When not in cooldown (yet), new state can take effect
  if (!cooldown) {
    if (stableState != value) {
      stableState = value;
      log(conf->name, value ? "1" : "0");
      uplink.setButton(conf->joy1.id, value);
    }
  }

  // Cooldown expires only when state idle
  if (lastState == value) {
    if (cooldown) {
      cooldown--;
    }
  } else {
    // Note we use only 4 bit for it, so max is 16.
    cooldown = 3;
  }
  lastState = value;
}

void Switch3Pos::setup(Uplink &uplink, const Switch3PosConf *conf) {
  uplink.setButton(conf->joy1.id, 0);
  uplink.setButton(conf->joy2.id, 1);
  uplink.setButton(conf->joy3.id, 0);
}

void Switch3Pos::loop(Uplink &uplink, const Switch3PosConf *conf,
                      uint8_t value1, uint8_t value2) {
  // Assuming only one half can be active at a time simplifies the code
  // because then we don't need to debounce independently.
  int value;
  if (value1) {
    value = 1;
  } else if (value2) {
    value = 3;
  } else {
    value = 2;
  }

  // When not in cooldown (yet), new state can take effect
  if (!cooldown) {
    if (stableState != value) {
      stableState = value;
      uplink.setButton(conf->joy1.id, stableState == 1);
      uplink.setButton(conf->joy2.id, stableState == 2);
      uplink.setButton(conf->joy3.id, stableState == 3);
      log(conf->name, stableState == 1 ? "1" : (stableState == 2 ? "2" : "3"));
    }
  }

  // Cooldown expires only when state idle
  if (lastState == value) {
    if (cooldown) {
      cooldown--;
    }
  } else {
    // Note we use only 4 bit for it, so max is 16.
    cooldown = 3;
  }
  lastState = value;
}

void RotaryEncoder::setup(Uplink &uplink, const RotaryEncoderConf *conf) {
  uplink.setButton(conf->joy1.id, 0);
  uplink.setButton(conf->joy2.id, 0);
}

void RotaryEncoder::loop(Uplink &uplink, const RotaryEncoderConf *conf,
                         uint8_t value1, uint8_t value2) {
  switch (decoder.process(value1, value2)) {
  case ROTARY_DECODER_EVENT_CCW | ROTARY_DECODER_EVENT_FULL:
    uplink.setButton(conf->joy1.id, 1);
    uplink.setButton(conf->joy2.id, 0);
    log(conf->name, "DEC");
    needReset = true;
    break;
  case ROTARY_DECODER_EVENT_CW | ROTARY_DECODER_EVENT_FULL:
    uplink.setButton(conf->joy1.id, 0);
    uplink.setButton(conf->joy2.id, 1);
    log(conf->name, "INC");
    needReset = true;
    break;
  default:
    if (needReset) {
      uplink.setButton(conf->joy1.id, 0);
      uplink.setButton(conf->joy2.id, 0);
      needReset = false;
    }
    break;
  }
}
