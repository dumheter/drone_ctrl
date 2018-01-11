#pragma once

// ============================================================ //
// Headers
// ============================================================ //

#include <cstdint>
#include "core/console.hpp"
#include "drone/serial.hpp"

// ============================================================ //
// Type Declaration
// ============================================================ //

/*
 * Used to decide how much a left turn should actually turn left.
 * On beginner, turning full left will only turn it slightly left.
 * While on master, turning full left, the dron will do its best.
 */
enum class drone_skill_level {
  DRONE_SKILL_LEVEL_BEGINNER = 0,
  DRONE_SKILL_LEVEL_AVERAGE,
  DRONE_SKILL_LEVEL_MASTER
};


constexpr uint8_t DRONE_CONTROL_INIT[] =     {0x66, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99};
constexpr uint8_t DRONE_CONTROL_MAINTAIN[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x99};
constexpr uint8_t DRONE_CONTROL_UNLOCK[] =   {0x66, 0x80, 0x80, 0x80, 0x80, 0x20, 0x20, 0x99};
constexpr uint8_t DRONE_CONTROL_STOP[] =     {0x66, 0x80, 0x80, 0x80, 0x80, 0x04, 0x04, 0x99};
constexpr uint8_t DRONE_CONTROL_SIZE = 8;
constexpr uint8_t DRONE_CONTROL_ALIGN_CHAR[] = {'c'};
const size_t      DRONE_CONTROL_ALIGN_CHAR_SIZE = 1;

constexpr unsigned long EXCLUSIVE_COMMAND_NOT_RUNNING = 0;

// ============================================================ //
// Class Declaration
// ============================================================ //

class Drone {
  
public:

  /*
  * @param port Example: "\\\\.\\COM11"
  * @param skill_level How much the drone will react to any command. 
  * For example, full forward will be different for different levels.
  */
  Drone(char port[], drone_skill_level skill_level = drone_skill_level::DRONE_SKILL_LEVEL_BEGINNER) : serial_(port) {
    set_state(DRONE_CONTROL_INIT);
    set_skill_level(skill_level);
    for (auto i = 0; i < DRONE_CONTROL_SIZE; i++) {
      exclusive_command_run_after_[i] = DRONE_CONTROL_INIT[i];
    }
  };

  /*
   * @param skill_level How much the drone will react to any command. 
   * For example, full forward will be different for different levels.
   */
  void set_skill_level(drone_skill_level skill_level);

  /*
   * Send drone state to connected serial device. Will also update the checksum.
   * Will only transmit data every x millisecond, some calls will be noop.
   */
  void send_state();

  /*
   * Read from serial device.
   * Will only transmit data every x millisecond, some calls will be noop.
   */
  void read_serial();

  /*
   * Update up down state
   * @param up_power How much we want to fly up
   * @param down_power How much we want to fly down.
   */
  void set_lift_power(const uint8_t up_power, const uint8_t down_power);

  /*
   * Update left right state
   */
  void set_left_right(int16_t left_right);

  /*
   * Update forward backward state
   */
  void set_forward_backward(int16_t forward_backward);

  /*
   * Update yaw
   */
  void set_yaw(bool left, bool right);

  /*
   * Are we connected to serial device?
   */
  bool is_connected() { return serial_.is_connected(); };

  /*
   * Disengage all motor functions by sending a STOP state for ~1 second.
   * Will engage the run_exlusive functionallity while doing it.
   */
  void stop_motors();

  /*
   * This is the default state for when engaging the motors and should be called before attempting to fly.
   */
  void start_motors();

  /*
   * Returns false when no change to state should be made. Will also release the "lock" when exlusive command is done.
   */
  bool run_exclusive_command();

  /*
   * Get state as binary string representation
   * example string: 0110 0110 | 1000 0001 | 0111 1000 | 1000 0000 | 1000 0000 | 0000 0000 | 1111 1001 | 1001 1001 
   */
  std::string get_state_as_binary_string();

private:

  Serial serial_;
  uint8_t state_[DRONE_CONTROL_SIZE]{};
  static constexpr unsigned int read_buf_size_ = 512;
  uint8_t read_buffer_[read_buf_size_];
  unsigned long exclusive_command_timeout_ = EXCLUSIVE_COMMAND_NOT_RUNNING;
  std::chrono::system_clock::time_point exclusive_command_timer_ = std::chrono::system_clock::now();
  uint8_t exclusive_command_run_after_[DRONE_CONTROL_SIZE]{};
  uint8_t skill_level_stearing_;
  uint8_t skill_level_lift_;

  /*
   * Overwrite all data bytes of state_ with new_state
   */
  void set_state(const uint8_t new_state[]);

  /*
   * After changing state you must update the checksum
   */
  void set_checksum();

  /*
   * Prepare run_exclusive_command to return false for the given time period.
   *
   * @param time_ms How long should run_exclusive_command return false for?
   */
  void new_exclusive_command(unsigned long time_ms, const uint8_t run_after[]);

};