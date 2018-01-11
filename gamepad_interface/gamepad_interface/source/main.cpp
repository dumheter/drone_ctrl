// ============================================================ //
// Headers
// ============================================================ //

#include "drone/gamepad.hpp"
#include "drone/drone.hpp"
#include "core/console.hpp"
#include <cstdint>

#include "core/helpers.hpp"

// ============================================================ //
// Main
// ============================================================ //

int main(int, char**) {

  Console::set_level(Logger::level::trace);

  char port[] = "\\\\.\\COM11";
  Drone drone{port, drone_skill_level::DRONE_SKILL_LEVEL_MASTER};
  Console::println("connected: {}", drone.is_connected());

  Gamepad gamepad{};

  while (true) {

    if (!drone.run_exclusive_command()) {

      if (!gamepad.update_state()) {
        Console::println(Logger::level::err, "Cannot find gamepad, is it connected?");
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // avoid clogging the logs with this message
      }

      if (gamepad.on_input_down(gamepad_input::GAMEPAD_BUTTON_B)) {
        Console::println("B - STOP");
        drone.stop_motors();
      }
      if (gamepad.on_input_down(gamepad_input::GAMEPAD_BUTTON_A)) {
        Console::println("A - UNLOCK");
        drone.start_motors();
      }

      if (gamepad.on_input_down(gamepad_input::GAMEPAD_BUTTON_LB)) {
        Console::println("LEFT SHOULDER - Yaw left");
      }
      if (gamepad.on_input_down(gamepad_input::GAMEPAD_BUTTON_RB)) {
        Console::println("RIGHT SHOULDER - Yaw right");
      }
      drone.set_yaw(gamepad.get_input_state(gamepad_input::GAMEPAD_BUTTON_LB),
                    gamepad.get_input_state(gamepad_input::GAMEPAD_BUTTON_RB));

      bool update_lift = false;
      if (gamepad.on_input_change(gamepad_input::GAMEPAD_TRIGGER_LT)) {
        update_lift = true;
      }
      if (gamepad.on_input_change(gamepad_input::GAMEPAD_TRIGGER_RT)) {
        update_lift = true;
      }
      if (update_lift) {
        drone.set_lift_power(static_cast<uint8_t>(gamepad.get_input_state(gamepad_input::GAMEPAD_TRIGGER_RT)),
                             static_cast<uint8_t>(gamepad.get_input_state(gamepad_input::GAMEPAD_TRIGGER_LT)));
      }

      if (gamepad.on_input_change(gamepad_input::GAMEPAD_STICK_LX)) {
        drone.set_left_right(static_cast<int16_t>(gamepad.get_input_state(gamepad_input::GAMEPAD_STICK_LX)));
      }
      if (gamepad.on_input_change(gamepad_input::GAMEPAD_STICK_LY)) {
        drone.set_forward_backward(static_cast<int16_t>(gamepad.get_input_state(gamepad_input::GAMEPAD_STICK_LY)));
      }

    }

    drone.send_state();
    drone.read_serial();
  }

  return 0;
}
