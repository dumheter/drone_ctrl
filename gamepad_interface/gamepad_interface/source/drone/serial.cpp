// ============================================================ //
// Headers
// ============================================================ //

#include "drone/serial.hpp"

// ============================================================ //
// Class Implementation
// ============================================================ //

serial_result_t Serial::read(uint8_t buffer[], const unsigned int buf_size, size_t& bytes_read) {
  bytes_read = _serial.readSerialPort(reinterpret_cast<char*>(buffer), buf_size);

  if (!bytes_read) {
    return serial_result_t::SERIAL_RESULT_NO_DATA;
  }

  return serial_result_t::SERIAL_RESULT_SUCCESS;
};

serial_result_t Serial::write(uint8_t buffer[], const unsigned int buf_size, size_t& bytes_read) {
  const auto bytes_written = _serial.writeSerialPort(reinterpret_cast<char*>(buffer), buf_size);
  if (bytes_written != buf_size) {
    //Console::println(Logger::level::err, "failed to write state to serial");
    return serial_result_t::SERIAL_RESULT_ERROR_UNKNOWN;
  }

  return serial_result_t::SERIAL_RESULT_SUCCESS;
};
