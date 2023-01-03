#include <Arduino.h>

#include <Wire.h>

#include "expander.h"

#define PCA9575_AUTOINCREMENT_REG 0x80

#define PCA9575_REG_INPUT_PORT_0 0x00
#define PCA9575_REG_INPUT_PORT_1 0x01
#define PCA9575_REG_POLARITY_INVERSION_PORT_0 0x02
#define PCA9575_REG_POLARITY_INVERSION_PORT_1 0x03
#define PCA9575_REG_BUS_HOLD_ENABLE_PORT_0 0x04
#define PCA9575_REG_BUS_HOLD_ENABLE_PORT_1 0x05
#define PCA9575_REG_PULL_UP_PULL_DOWN_SELECT_PORT_0 0x06
#define PCA9575_REG_PULL_UP_PULL_DOWN_SELECT_PORT_1 0x07
#define PCA9575_REG_CONFIGURATION_PORT_0 0x08
#define PCA9575_REG_CONFIGURATION_PORT_1 0x09
#define PCA9575_REG_OUTPUT_PORT_0 0x0A
#define PCA9575_REG_OUTPUT_PORT_1 0x0B
#define PCA9575_REG_INTERRUPT_MASK_PORT_0 0x0C
#define PCA9575_REG_INTERRUPT_MASK_PORT_1 0x0D
#define PCA9575_REG_INTERRUPT_STATUS_PORT_0 0x0E
#define PCA9575_REG_INTERRUPT_STATUS_PORT_1 0x0F

// Reset all PCA9575 on the bus with via a broadcast command.
// See: PCA9575 data sheet, section 7.9 Software Reset.
void PCA9575::resetAll() {
  Wire.beginTransmission(0x00);
  Wire.write(0x06);
  Wire.endTransmission();
}

void PCA9575::setup(const PCA9575Conf *conf) {
  Wire.beginTransmission(conf->addr);
  Wire.write(PCA9575_REG_BUS_HOLD_ENABLE_PORT_0 | PCA9575_AUTOINCREMENT_REG);
  Wire.write(0x02); // PCA9575_REG_BUS_HOLD_ENABLE_PORT_0, Enable
                    // pull-up/pull-down, disable bus-hold
  Wire.write(0x02); // PCA9575_REG_BUS_HOLD_ENABLE_PORT_1, Enable
                    // pull-up/pull-down, disable bus-hold
  Wire.write(0xFF); // PCA9575_REG_PULL_UP_PULL_DOWN_SELECT_PORT_0, Select
                    // pull-ups for all pins
  Wire.write(0xFF); // PCA9575_REG_PULL_UP_PULL_DOWN_SELECT_PORT_1, Select
                    // pull-ups for all pins
  Wire.write(
      0xFF); // PCA9575_REG_CONFIGURATION_PORT_0, Configure as input (for now)
  Wire.write(
      0xFF); // PCA9575_REG_CONFIGURATION_PORT_1, Configure as input (for now)
  Wire.write(0x00); // PCA9575_REG_OUTPUT_PORT_0, Output LOW (if/when pin
                    // configured as output - used for matrix)
  Wire.write(0x00); // PCA9575_REG_OUTPUT_PORT_1, Output LOW (if/when pin
                    // configured as output - used for matrix)
  Wire.endTransmission();
}

int PCA9575::pollPortsAsArray(const PCA9575Conf *conf) {
  int ret = 0; // Success

  Wire.beginTransmission(conf->addr);
  Wire.write(PCA9575_REG_INPUT_PORT_0 | PCA9575_AUTOINCREMENT_REG);
  Wire.endTransmission(false);

  Wire.requestFrom(conf->addr, (uint8_t)1, (uint8_t) false);
  if (Wire.available()) {
    state[0][0] = Wire.read();
  } else {
    ret = 1;
  }

  Wire.requestFrom(conf->addr, (uint8_t)1, (uint8_t) true);
  if (Wire.available()) {
    state[1][0] = Wire.read();
  } else {
    ret = 1;
  }

  return ret;
}

int PCA9575::pollPortAsArray(const PCA9575Conf *conf, int port) {
  int ret = 0; // Success

  Wire.beginTransmission(conf->addr);
  Wire.write(PCA9575_REG_INPUT_PORT_0 + port);
  Wire.endTransmission(false);

  Wire.requestFrom(conf->addr, (uint8_t)1);
  if (Wire.available()) {
    state[port][0] = Wire.read();
    // Serial.println(state[port][0]); // DEBUG
  } else {
    ret = 1; // Error
  }

  return ret;
}

int PCA9575::pollPortsAsMatrix(const PCA9575Conf *conf) {
  int ret = 0; // Success

  for (int column = 0; column < 8; column++) {
    ret |= pollPortsAsMatrixColumn(conf, column);
  }

  return ret;
}

int PCA9575::pollPortAsMatrix(const PCA9575Conf *conf, int port) {
  int ret = 0; // Success

  for (int column = 0; column < 8; column++) {
    ret |= pollPortAsMatrixColumn(conf, port, column);
  }

  return ret;
}

int PCA9575::pollPortsAsMatrixColumn(const PCA9575Conf *conf, int column) {
  int ret = 0;

  // Put one column in output mode; the others in input mode.
  // We have already configured the output value to be LOW in setup().
  Wire.beginTransmission(conf->addr);
  Wire.write(PCA9575_REG_CONFIGURATION_PORT_0 | PCA9575_AUTOINCREMENT_REG);
  Wire.write(~(1 << column)); // PCA9575_REG_CONFIGURATION_PORT_0
  Wire.write(~(1 << column)); // PCA9575_REG_CONFIGURATION_PORT_1
  Wire.endTransmission();

  // Read inputs.
  Wire.beginTransmission(conf->addr);
  Wire.write(PCA9575_REG_INPUT_PORT_0 | PCA9575_AUTOINCREMENT_REG);
  Wire.endTransmission(false);
  Wire.requestFrom(conf->addr, (uint8_t)(uint8_t)1, (uint8_t) false);
  if (Wire.available()) {
    state[0][column] = Wire.read(); // PCA9575_REG_INPUT_PORT_0
  } else {
    ret = 1;
  }
  Wire.requestFrom(conf->addr, (uint8_t)1, (uint8_t) true);
  if (Wire.available()) {
    state[1][column] = Wire.read(); // PCA9575_REG_INPUT_PORT_1
  } else {
    ret = 1;
  }

  return ret;
}

int PCA9575::pollPortAsMatrixColumn(const PCA9575Conf *conf, int port,
                                    int column) {
  int ret = 0;

  Wire.beginTransmission(conf->addr);
  Wire.write(PCA9575_REG_CONFIGURATION_PORT_0 + port);
  Wire.write(~(1 << column));
  Wire.endTransmission();

  Wire.beginTransmission(conf->addr);
  Wire.write(PCA9575_REG_INPUT_PORT_0 + port);
  Wire.endTransmission();
  Wire.requestFrom(conf->addr, (uint8_t)1);
  if (Wire.available()) {
    state[port][column] = Wire.read();
  } else {
    ret = 1;
  }

  return ret;
}

int PCA9575::loop(const PCA9575Conf *conf, uint8_t column) {
  int ret = 0;
  if (conf->mode[0] == PCA9575_MODE_ARRAY &&
      conf->mode[1] == PCA9575_MODE_ARRAY) {
    ret |= pollPortsAsArray(conf);
  } else if (conf->mode[0] == PCA9575_MODE_MATRIX &&
             conf->mode[1] == PCA9575_MODE_MATRIX) {
    ret |= pollPortsAsMatrixColumn(conf, column);
  } else {
    if (conf->mode[0] == PCA9575_MODE_ARRAY) {
      ret |= pollPortAsArray(conf, 0);
    } else if (conf->mode[0] == PCA9575_MODE_MATRIX) {
      ret |= pollPortAsMatrixColumn(conf, 0, column);
    }
    if (conf->mode[1] == PCA9575_MODE_ARRAY) {
      ret |= pollPortAsArray(conf, 1);
    } else if (conf->mode[1] == PCA9575_MODE_MATRIX) {
      ret |= pollPortAsMatrixColumn(conf, 1, column);
    }
  }
  return ret;
}
