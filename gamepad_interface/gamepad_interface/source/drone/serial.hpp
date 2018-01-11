#pragma once

// ============================================================ //
// Headers
// ============================================================ //

#include "thirdparty/SerialPort/SerialPort.h"
#include <string>
#include <cstdint>
#include <chrono>

// ============================================================ //
// Type Declaration
// ============================================================ //

enum class serial_result_t {
  SERIAL_RESULT_SUCCESS = 0,
  SERIAL_RESULT_ERROR_UNKNOWN,
  SERIAL_RESULT_NOT_CONNECTED,
  SERIAL_RESULT_NO_DATA
};

// ============================================================ //
// Class Declaration
// ============================================================ //


class Serial {

public:

  /*
   * Will initialize the port
   *
   * @param port Example: "\\\\.\\COM11"
   */
  Serial(char port[]) : _serial(port) {};

  /*
   * @param
   */
  serial_result_t read(uint8_t buffer[], const unsigned int buf_size, size_t& bytes_read);


  serial_result_t write(uint8_t buffer[], const unsigned int buf_size, size_t& bytes_read);

  bool is_connected() { return _serial.isConnected(); };

private:

  SerialPort _serial;

};

