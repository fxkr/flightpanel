#include <Wire.h>

#include "expander.h"
#include "input.h"
#include "usb.h"

Uplink uplink;

// Static part of PCA9575 objects goes in flash (PROGMEM).
// Arrangement of expanders:
const PROGMEM PCA9575Conf expanderConfProgmem[] = {
    // Bottom left
    {0b0100000, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},
    {0b0100001, {PCA9575_MODE_MATRIX, PCA9575_MODE_NONE}},
    {0b0100010, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},

    // Top left
    {0b0100011, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},
    {0b0100100, {PCA9575_MODE_NONE, PCA9575_MODE_NONE}},
    {0b0100101, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},

    // Bottom right
    {0b0100110, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},
    {0b0100111, {PCA9575_MODE_MATRIX, PCA9575_MODE_NONE}},
    {0b0101000, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},

    // Mid
    {0b0101001, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},
    {0b0101010, {PCA9575_MODE_MATRIX, PCA9575_MODE_MATRIX}},
    {0b0101011, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},

    // // Top right
    {0b0101100, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},
    {0b0101101, {PCA9575_MODE_NONE, PCA9575_MODE_NONE}},
    {0b0101110, {PCA9575_MODE_ARRAY, PCA9575_MODE_ARRAY}},
};

// Dynamic part.
// These classes have been optimized to use minimal RAM; usually 1 byte per
// object.
PCA9575 expanders[sizeof(expanderConfProgmem) / sizeof(expanderConfProgmem)[0]];

#include "input_defs.h"

void setupExpander(unsigned int i) {
  PCA9575Conf conf = {};
  memcpy_P(&conf, &expanderConfProgmem[i], sizeof(expanderConfProgmem[i]));
  expanders[i].setup(&conf);
}

void setup() {
  // Initialize Wire very early to avoid issues with I2C and USB.
  // https://forum.arduino.cc/t/i2c-and-usb-conflict-on-atmega32u4-with-leonardo-bootloader/250080/4
  Wire.begin();

  Serial.begin(500000); // = 14400 bytes/s

  delay(50);

  Wire.setClock(400000);

#if DEBUG_ENABLE_TIMING_PIN
  pinMode(DEBUG_ENABLE_TIMING_PIN, OUTPUT);
  digitalWrite(DEBUG_ENABLE_TIMING_PIN, LOW);
#endif

  uplink.setup();

  delay(50);

  PCA9575::resetAll();

  for (unsigned int i = 0; i < sizeof(expanders) / sizeof(expanders[0]); i++) {
    setupExpander(i);
  }
}

void loopExpander(unsigned int i, int column) {
  PCA9575Conf conf = {};
  memcpy_P(&conf, &expanderConfProgmem[i], sizeof(expanderConfProgmem[i]));
  expanders[i].loop(&conf, column);
}

void loopExpanders() {
  static int column = 0;
  column = (column + 1) % 8;
  for (unsigned int i = 0; i < sizeof(expanders) / sizeof(expanders[0]); i++) {
    loopExpander(i, column);
  }
}

void debugDiffExpanders() {
  static uint8_t state[sizeof(expanders) / sizeof(expanders[0])][2][8];
  static bool firstDone = false;

  for (unsigned int expanderIdx = 0;
       expanderIdx < sizeof(expanders) / sizeof(expanders[expanderIdx]);
       expanderIdx++) {
    for (unsigned int portIdx = 0; portIdx < 2; portIdx++) {
      for (unsigned int colIdx = 0; colIdx < 8; colIdx++) {
        for (unsigned int pinIdx = 0; pinIdx < 8; pinIdx++) {
          bool prev = !!(state[expanderIdx][portIdx][colIdx] & (1 << pinIdx));
          bool cur =
              !!(expanders[expanderIdx].state[portIdx][colIdx] & (1 << pinIdx));

          if (prev != cur) {
            if (firstDone) {
              Serial.print(cur ? "+\tExp:" : "-\tExp:");
              Serial.print(expanderIdx);
              Serial.print("\tPort:");
              Serial.print(portIdx);
              Serial.print("\tCol:");
              Serial.print(colIdx);
              Serial.print("\tPin");
              Serial.print(pinIdx);
              Serial.print("\t");
              Serial.print(prev);
              Serial.print(" -> ");
              Serial.print(cur);
              Serial.print("\r\n");
            }

            state[expanderIdx][portIdx][colIdx] &= ~(1 << pinIdx);
            state[expanderIdx][portIdx][colIdx] |= (cur << pinIdx);
          }
        }
      }
    }
  }

  for (unsigned int i = 0; i < sizeof(expanders) / sizeof(expanders[0]); i++) {
    memcpy(state[i], expanders[i].state, sizeof(state[i]));
  }

  firstDone = true;
}

void debugHeartbeat() {
  static unsigned long lastHeartbeat = 0;
  unsigned long now = millis();

  if (now - lastHeartbeat > 500) {
    Serial.write('.');
    Serial.flush();
    lastHeartbeat = now;
  }
}

void loop() {
#if DEBUG_ENABLE_TIMING_PIN
  digitalWrite(DEBUG_ENABLE_TIMING_PIN, HIGH);
#endif

#if DEBUG_ENABLE_SERIAL_HEARTBEAT
  debugHeartbeat();
#endif

  loopExpanders();

#if DEBUG_ENABLE_EXPANDER_LOG
  debugDiffExpanders();
#endif

  loopInputs();
  uplink.loop();

#if DEBUG_ENABLE_TIMING_PIN
  digitalWrite(DEBUG_ENABLE_TIMING_PIN, LOW);
#endif
}
