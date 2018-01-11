#pragma once

// ============================================================ //
// Headers
// ============================================================ //

#include <windows.h>
#include <Xinput.h>
#include <cstdint>

// ============================================================ //
// Type Declarations
// ============================================================ //

enum class gamepad_input : uint8_t {
  // IMPORTANT update GAMEPAD_INPUT_LENGTH if adding/removing
  GAMEPAD_BUTTON_A = 0,
  GAMEPAD_BUTTON_B,
  GAMEPAD_BUTTON_X,
  GAMEPAD_BUTTON_Y,
  GAMEPAD_BUTTON_LB,
  GAMEPAD_BUTTON_RB,
  GAMEPAD_TRIGGER_LT,
  GAMEPAD_TRIGGER_RT,
  GAMEPAD_STICK_LX,
  GAMEPAD_STICK_LY,
  GAMEPAD_STICK_RX,
  GAMEPAD_STICK_RY
};

// IMPORTANT update GAMEPAD_INPUT_LENGTH if adding/removing
static constexpr uint16_t GAMEPAD_INPUT_LENGTH = 12;

static constexpr uint16_t GAMEPAD_BUTTON_STATE_RELEASED = 0;
static constexpr uint16_t GAMEPAD_BUTTON_STATE_PRESSED = 1;

// ============================================================ //
// Gamepad Class Declaration
// ============================================================ //

class Gamepad {

public:

  /*
   * @param gamepad_index The first gamepad will have an index of 0. Second 1 and so on.
   */
  Gamepad(const uint8_t gamepad_index = 0) : gamepad_index(gamepad_index) {};

  /*
   * @return If we succeded to update state or not.
   * If not, it's likely the controller is not connected.
   */
  bool update_state();

  /*
   * Only works on inputs that are considered buttons. Will only return true
   * when the button first is pressed and may only return true again after realsing it first.
   */
  bool on_input_down(gamepad_input input);

  /*
   * Use for analog input such as trigger and sticks. Will return
   * a value only when the value is not the same as last call.
   */
  bool on_input_change(gamepad_input input);

  /*
   * @return Raw value of the button/analog input.
   */
  uint16_t get_input_state(gamepad_input input) const { return gamepad_state[static_cast<uint8_t>(input)]; };

private:

  XINPUT_STATE input_state{};
  uint16_t gamepad_state[GAMEPAD_INPUT_LENGTH]{};
  const uint8_t gamepad_index;

};
