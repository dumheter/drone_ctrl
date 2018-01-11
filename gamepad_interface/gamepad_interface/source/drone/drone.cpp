// ============================================================ //
// Headers
// ============================================================ //

#include "drone/drone.hpp"
#include "core/helpers.hpp"
#include "core/console.hpp"

// ============================================================ //
// Class Implementation
// ============================================================ //

/*
 * Keep the level between 0 - 127.
 */
void Drone::set_skill_level(drone_skill_level skill_level) {
  switch (skill_level) {
  case drone_skill_level::DRONE_SKILL_LEVEL_BEGINNER:
    skill_level_lift_ = 100;
    skill_level_stearing_ = 55;
    break;

  case drone_skill_level::DRONE_SKILL_LEVEL_AVERAGE:
    skill_level_lift_ = 120;
    skill_level_stearing_ = 80;
    break;

  case drone_skill_level::DRONE_SKILL_LEVEL_MASTER:
    skill_level_lift_ = 127;
    skill_level_stearing_ = 127;
  }
}

// ============================================================ //

void Drone::send_state() {
  static auto timer = std::chrono::system_clock::now(); 
  if (!Helpers::run_interval(timer, 50UL)) return;
  
  set_checksum();

  static uint8_t tmp_align_char[] = {DRONE_CONTROL_ALIGN_CHAR[0]};
  size_t bytes_written;
  auto result = serial_.write(tmp_align_char, DRONE_CONTROL_ALIGN_CHAR_SIZE, bytes_written);

  if (result == serial_result_t::SERIAL_RESULT_SUCCESS) {
    result = serial_.write(state_, DRONE_CONTROL_SIZE, bytes_written);

    if (result == serial_result_t::SERIAL_RESULT_SUCCESS) {
      Console::println(Logger::level::debug, "writing state: {}", get_state_as_binary_string());
    }
    else {
      Console::println(Logger::level::err, "Failed to write to serial, wrong port provided or not connected?");
    }
  }
  else {
    Console::println(Logger::level::err, "Failed to write to serial, wrong port provided or not connected?");
  }
}

// ============================================================ //

void Drone::read_serial() {
  static auto timer = std::chrono::system_clock::now();
  if (!Helpers::run_interval(timer, 1UL)) return; //avoids reading before everything is transmitted

  size_t bytes_read;
  const auto result = serial_.read(read_buffer_, read_buf_size_, bytes_read);
  if (result == serial_result_t::SERIAL_RESULT_SUCCESS) {
    read_buffer_[bytes_read > read_buf_size_ ? read_buf_size_ - 1 : bytes_read] = 0; //null terminate to print
    
    auto log_level = Logger::level::debug;
    if (read_buffer_[0] == 'E' && read_buffer_[1] == ':') {
      log_level = Logger::level::err;
    }
    else if (read_buffer_[0] == 'W' && read_buffer_[1] == ':') {
      log_level = Logger::level::warn;
    }

    Console::println(log_level, "Serial read, len {}\tdata: {}", bytes_read, reinterpret_cast<char*>(read_buffer_));
  }
  else {
    //Console::println(Logger::level::err, "failed to read serial - {}", bytes_read);
  }
}

// ============================================================ //

void Drone::set_lift_power(const uint8_t up_power, const uint8_t down_power) {
  // protect agains analog input injections when running exlusive commands
  if (exclusive_command_timeout_ != EXCLUSIVE_COMMAND_NOT_RUNNING) return;

  const uint8_t up = Helpers::map(up_power, static_cast<uint8_t>(0), static_cast<uint8_t>(255), static_cast<uint8_t>(0), skill_level_lift_);
  const uint8_t down = Helpers::map(down_power, static_cast<uint8_t>(0), static_cast<uint8_t>(255), static_cast<uint8_t>(0), skill_level_lift_);
  const uint8_t net_power = up >= down ? 128 + up - down: 127 + up - down;

  state_[3] = net_power;
}

// ============================================================ //

void Drone::set_left_right(int16_t left_right) {
  // protect agains analog input injections when running exlusive commands
  if (exclusive_command_timeout_ != EXCLUSIVE_COMMAND_NOT_RUNNING) return;

  const bool left = left_right & 0b1000000000000000;
  uint8_t byte_left_right;

  if (left) {
    byte_left_right = 127 - static_cast<uint8_t>(Helpers::map(
      static_cast<int16_t>(-left_right), static_cast<int16_t>(0), static_cast<int16_t>(32767),
      static_cast<int16_t>(0), static_cast<int16_t>(skill_level_stearing_)));
  }
  else {
    byte_left_right = 128 + static_cast<uint8_t>(Helpers::map(
      left_right, static_cast<int16_t>(0), static_cast<int16_t>(32767),
      static_cast<int16_t>(0), static_cast<int16_t>(skill_level_stearing_)));
  }

  state_[1] = byte_left_right;
}

// ============================================================ //

void Drone::set_forward_backward(int16_t forward_backward) {
  // protect agains analog input injections when running exlusive commands
  if (exclusive_command_timeout_ != EXCLUSIVE_COMMAND_NOT_RUNNING) return;

  const bool backward = forward_backward & 0b1000000000000000;
  uint8_t byte_forward_backward;

  if (backward) {
    byte_forward_backward = 127 - static_cast<uint8_t>(Helpers::map(
      static_cast<int16_t>(-forward_backward), static_cast<int16_t>(0), static_cast<int16_t>(32767),
      static_cast<int16_t>(0), static_cast<int16_t>(skill_level_stearing_)));
  }
  else {
    byte_forward_backward = 128 + static_cast<uint8_t>(Helpers::map(
      forward_backward, static_cast<int16_t>(0), static_cast<int16_t>(32767), 
        static_cast<int16_t>(0), static_cast<int16_t>(skill_level_stearing_)));
  }

  state_[2] = byte_forward_backward;
}

// ============================================================ //

void Drone::set_yaw(bool left, bool right) {
  // protect agains analog input injections when running exlusive commands
  if (exclusive_command_timeout_ != EXCLUSIVE_COMMAND_NOT_RUNNING) return;

  const uint8_t adjusted_left = Helpers::map(static_cast<uint8_t>(left), static_cast<uint8_t>(0), static_cast<uint8_t>(1), static_cast<uint8_t>(0), skill_level_stearing_);
  const uint8_t adjusted_right = Helpers::map(static_cast<uint8_t>(right), static_cast<uint8_t>(0), static_cast<uint8_t>(1), static_cast<uint8_t>(0), skill_level_stearing_);

  const uint8_t net_power = adjusted_left >= adjusted_right ? 128 - adjusted_left + adjusted_right : 127 - adjusted_left + adjusted_right;

  state_[4] = net_power;
}

// ============================================================ //

void Drone::stop_motors() {
  set_state(DRONE_CONTROL_STOP);
  new_exclusive_command(1100UL, DRONE_CONTROL_INIT);
}

// ============================================================ //

void Drone::start_motors() {
  // protect agains analog input injections when running exlusive commands
  if (exclusive_command_timeout_ != EXCLUSIVE_COMMAND_NOT_RUNNING) return;

  set_state(DRONE_CONTROL_UNLOCK);
  new_exclusive_command(600UL, DRONE_CONTROL_MAINTAIN);
}

// ============================================================ //

bool Drone::run_exclusive_command() {
  using namespace std::chrono;

  if (!exclusive_command_timeout_) {
    return false;
  }
  else if (duration_cast<milliseconds>(system_clock::now() - exclusive_command_timer_).count() >= exclusive_command_timeout_) {
    exclusive_command_timer_ = system_clock::now();
    exclusive_command_timeout_ = EXCLUSIVE_COMMAND_NOT_RUNNING;
    set_state(exclusive_command_run_after_);
    Console::println(Logger::level::warn, "Setting state after exclusive command");
    return false;
  }
  else {
    return true;
  }
}

// ============================================================ //

std::string Drone::get_state_as_binary_string() {
  std::string state;
  for (auto byte_index = 0; byte_index < DRONE_CONTROL_SIZE; byte_index++) {
    for (auto space_after_4 = 0; space_after_4 < 2; space_after_4++) {
      for (auto nibble = 0; nibble < 4; nibble++) {
        state += std::to_string((0 != (state_[byte_index] & (1UL << (7 - (4 * space_after_4 + nibble))))));
      }
      state += " ";
    }
    state += "| ";
  }
  return state;
}

// ============================================================ //

void Drone::set_state(const uint8_t new_state[]) {
  for (uint8_t i = 0; i < DRONE_CONTROL_SIZE; i++) {
    state_[i] = new_state[i];
  }
}

// ============================================================ //

void Drone::set_checksum() {
  state_[6] = state_[1] ^ state_[2] ^ state_[3] ^ state_[4] ^ state_[5];
}

// ============================================================ //

void Drone::new_exclusive_command(unsigned long time_ms, const uint8_t run_after[]) {
  if (time_ms == 0) return;

  exclusive_command_timeout_ = time_ms;
  exclusive_command_timer_ = std::chrono::system_clock::now();

  for (auto i = 0; i < DRONE_CONTROL_SIZE; i++) {
    exclusive_command_run_after_[i] = run_after[i];
  }
}