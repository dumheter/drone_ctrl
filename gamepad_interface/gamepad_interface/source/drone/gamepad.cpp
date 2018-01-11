// ============================================================ //
// Headers
// ============================================================ //

#include "drone/gamepad.hpp"

// ============================================================ //
// Class Implementation
// ============================================================ //

bool Gamepad::update_state() {
  return XInputGetState(gamepad_index, &input_state) == ERROR_SUCCESS;
}

// ============================================================ //

bool Gamepad::on_input_down(gamepad_input input) {

  switch (input) {
  case gamepad_input::GAMEPAD_BUTTON_A:
    if (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_A &&
        gamepad_state[static_cast<uint8_t>(input)] != GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_PRESSED;
      return true;
    }
    else if (!(input_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
             gamepad_state[static_cast<uint8_t>(input)] == GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_RELEASED;
    }
    break;

  case gamepad_input::GAMEPAD_BUTTON_B:
    if (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_B &&
        gamepad_state[static_cast<uint8_t>(input)] != GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_PRESSED;
      return true;
    }
    else if (!(input_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
             gamepad_state[static_cast<uint8_t>(input)] == GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_RELEASED;
    }
    break;

  case gamepad_input::GAMEPAD_BUTTON_X:
    if (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_X &&
        gamepad_state[static_cast<uint8_t>(input)] != GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_PRESSED;
      return true;
    }
    else if (!(input_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) &&
             gamepad_state[static_cast<uint8_t>(input)] == GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_RELEASED;
    }
    break;

  case gamepad_input::GAMEPAD_BUTTON_Y:
    if (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y &&
        gamepad_state[static_cast<uint8_t>(input)] != GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_PRESSED;
      return true;
    }
    else if (!(input_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) &&
             gamepad_state[static_cast<uint8_t>(input)] == GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_RELEASED;
    }
    break;

  case gamepad_input::GAMEPAD_BUTTON_LB:
    if (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER &&
        gamepad_state[static_cast<uint8_t>(input)] != GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_PRESSED;
      return true;
    }
    else if (!(input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) &&
             gamepad_state[static_cast<uint8_t>(input)] == GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_RELEASED;
    }
    break;

  case gamepad_input::GAMEPAD_BUTTON_RB:
    if (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER &&
        gamepad_state[static_cast<uint8_t>(input)] != GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_PRESSED;
      return true;
    }
    else if (!(input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
             gamepad_state[static_cast<uint8_t>(input)] == GAMEPAD_BUTTON_STATE_PRESSED) {
      gamepad_state[static_cast<uint8_t>(input)] = GAMEPAD_BUTTON_STATE_RELEASED;
    }
    break;

  default:
    return false;
  }

  return false;
}

// ============================================================ //

bool Gamepad::on_input_change(gamepad_input input) {
  bool anything_changed = false;

  switch (input) {
  case gamepad_input::GAMEPAD_TRIGGER_RT:
    if (input_state.Gamepad.bRightTrigger != gamepad_state[static_cast<uint8_t>(input)]) {
      gamepad_state[static_cast<uint8_t>(input)] = input_state.Gamepad.bRightTrigger;
      anything_changed = true;
    }
    break;

  case gamepad_input::GAMEPAD_TRIGGER_LT:
    if (input_state.Gamepad.bLeftTrigger != gamepad_state[static_cast<uint8_t>(input)]) {
      gamepad_state[static_cast<uint8_t>(input)] = input_state.Gamepad.bLeftTrigger;
      anything_changed = true;
    }
    break;

  case gamepad_input::GAMEPAD_STICK_LX:
    if (input_state.Gamepad.sThumbLX != gamepad_state[static_cast<uint8_t>(input)]) {
      gamepad_state[static_cast<uint8_t>(input)] = input_state.Gamepad.sThumbLX;
      anything_changed = true;
    }
    break;

  case gamepad_input::GAMEPAD_STICK_LY:
    if (input_state.Gamepad.sThumbLY != gamepad_state[static_cast<uint8_t>(input)]) {
      gamepad_state[static_cast<uint8_t>(input)] = input_state.Gamepad.sThumbLY;
      anything_changed = true;
    }
    break;
  }

  return anything_changed;
}
