#pragma once

#include <Arduino.h>

class RotaryDecoder {
public:
  RotaryDecoder();
  int process(int pin1state, int pin2state);

private:
  uint8_t state;
};

#define ROTARY_DECODER_EVENT_CW 0x10
#define ROTARY_DECODER_EVENT_CCW 0x20
#define ROTARY_DECODER_EVENT_HALF 0x40
#define ROTARY_DECODER_EVENT_FULL 0x80