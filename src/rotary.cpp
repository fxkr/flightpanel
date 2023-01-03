#include <Arduino.h>

#include "rotary.h"

#define ROTARY_DECODER_STATE_START 0x0
#define ROTARY_DECODER_STATE_CW_1 0x1
#define ROTARY_DECODER_STATE_CW_2 0x2
#define ROTARY_DECODER_STATE_CW_3 0x3
#define ROTARY_DECODER_STATE_CCW_1 0x4
#define ROTARY_DECODER_STATE_CCW_2 0x5
#define ROTARY_DECODER_STATE_CCW_3 0x6

// Each row is a state.
// Each column is a condition for the input pins, in order: 00, 01, 10, 11.
// Each cell is a transition, consisting of a next state, optionally OR'ed with
// an event to return.
//
// Invalid conditions go back to start.
// The condition that led to a state will stay in the state (but an event will
// not be yielded again). Valid conditions go back to the next state in the
// clockwise or counter-clockwise state sequence.
const PROGMEM uint8_t rotary_decoder_state_table[7][4] = {
    // ROTARY_DECODER_STATE_START (00)
    {ROTARY_DECODER_STATE_START, ROTARY_DECODER_STATE_CW_1,
     ROTARY_DECODER_STATE_CCW_1, ROTARY_DECODER_STATE_START},

    // ROTARY_DECODER_STATE_CW_1 (01)
    {ROTARY_DECODER_STATE_START, ROTARY_DECODER_STATE_CW_1,
     ROTARY_DECODER_STATE_START,
     ROTARY_DECODER_STATE_CW_2 | ROTARY_DECODER_EVENT_CW |
         ROTARY_DECODER_EVENT_HALF},

    // ROTARY_DECODER_STATE_CW_2 (11)
    {ROTARY_DECODER_STATE_START, ROTARY_DECODER_STATE_START,
     ROTARY_DECODER_STATE_CW_3, ROTARY_DECODER_STATE_CW_2},

    // ROTARY_DECODER_STATE_CW_3 (10)
    {ROTARY_DECODER_STATE_START | ROTARY_DECODER_EVENT_CW |
         ROTARY_DECODER_EVENT_FULL,
     ROTARY_DECODER_STATE_START, ROTARY_DECODER_STATE_CW_3,
     ROTARY_DECODER_STATE_START},

    // ROTARY_DECODER_STATE_CCW_1 (10)
    {ROTARY_DECODER_STATE_START, ROTARY_DECODER_STATE_START,
     ROTARY_DECODER_STATE_CCW_1,
     ROTARY_DECODER_STATE_CCW_2 | ROTARY_DECODER_EVENT_CCW |
         ROTARY_DECODER_EVENT_HALF},

    // ROTARY_DECODER_STATE_CCW_2 (11)
    {ROTARY_DECODER_STATE_START, ROTARY_DECODER_STATE_CCW_3,
     ROTARY_DECODER_STATE_START, ROTARY_DECODER_STATE_CCW_2},

    // ROTARY_DECODER_STATE_CCW_3 (01)
    {ROTARY_DECODER_STATE_START | ROTARY_DECODER_EVENT_CCW |
         ROTARY_DECODER_EVENT_FULL,
     ROTARY_DECODER_STATE_CCW_3, ROTARY_DECODER_STATE_START,
     ROTARY_DECODER_STATE_START},
};

RotaryDecoder::RotaryDecoder() { state = ROTARY_DECODER_STATE_START; }

// Update rotary decoder state.
//
// Clockwise/counter-clockwise are arbitrary conventions.
// If it goes the wrong way, try swapping pin1state and pin2state.
//
// Expects active-high signals.
// If it doesn't seem to work at all, try negating both pin1state and pin2state.
//
// Inputs must be 0 or 1 only.
//
// Returns an OR'ed value consisting of:
//
//   ROTARY_DECODER_EVENT_CW - Event is in clock-wise direction
//   ROTARY_DECODER_EVENT_CCW - Event is in counter clock-wise direction.
//
// And:
//
//   ROTARY_DECODER_EVENT_HALF - Event is a half step.
//   ROTARY_DECODER_EVENT_FULL - Event is a full step.
//
// Or 0, if no event has happened.
int RotaryDecoder::process(int pin1state, int pin2state) {
  uint8_t value = pgm_read_byte(
      &rotary_decoder_state_table[state][(pin1state << 1) | pin2state]);
  uint8_t newState = value & 0xF;
  uint8_t event = value & 0xF0;

  state = newState;
  return event;
}
