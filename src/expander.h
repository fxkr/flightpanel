#pragma once

#include <Arduino.h>

#define PCA9575_MODE_NONE 0
#define PCA9575_MODE_ARRAY 1
#define PCA9575_MODE_MATRIX 2

class PCA9575Conf {
public:
  // 7-bit I2C address. valid values are 0-127
  // (because it dose not include the read/write bit).
  const uint8_t addr;

  // Mode to drive each port as. One of:
  // - PCA9575_MODE_NONE
  // - PCA9575_MODE_ARRAY
  // - PCA9575_MODE_MATRIX
  const uint8_t mode[2];
};

// NXP's PCA9575 is a 16-bit (2 ports of 8 bit each) I2C-bus GPIO.
//
// This implementation features:
//
// - Functions for optimal access to one or both ports.
//
// - Treating each port either as a flat array of inputs,
//   or as a charlieplexing matrix.
//
// - Software reset broadcast.
class PCA9575 {
public:
  // Poll results.
  // 1st dimension is the port.
  // 2nd dimension: when port is polled as:
  //   array: first element holds pin values as bits.
  //   matrix: each element holds pin values for one column as bits.
  //           The bit corresponding to the column itself is invalid.
  uint8_t state[2][8];

  static void resetAll();

  void setup(const PCA9575Conf *conf);

  int pollPortsAsArray(const PCA9575Conf *conf);
  int pollPortAsArray(const PCA9575Conf *conf, int port);

  int pollPortsAsMatrix(const PCA9575Conf *conf);
  int pollPortAsMatrix(const PCA9575Conf *conf, int port);

  int pollPortsAsMatrixColumn(const PCA9575Conf *conf, int column);
  int pollPortAsMatrixColumn(const PCA9575Conf *conf, int port, int column);

  int loop(const PCA9575Conf *conf, uint8_t column);
};