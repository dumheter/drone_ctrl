#pragma once

// ============================================================ //
// Functions
// ============================================================ //

namespace Helpers {

  // thanks https://www.arduino.cc/reference/en/language/functions/math/map/
  template <typename TYPE>
  TYPE map(TYPE x, TYPE in_min, TYPE in_max, TYPE out_min, TYPE out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  // thanks https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
  template <typename Type>
  void set_bit(Type& variable, Type index, Type new_value) {
    variable ^= (-new_value ^ variable) & (1UL << index);
  }

  /*
  * Check if you can run your function
  *
  * Example (run each 10 millisecond):
  * static auto timer = system_clock::now();
  * if (!run_interval(timer, 10)) return;
  *
  * @param timer Store a static variable like this: static auto timer = system_clock::now();
  *
  * @retval true Interval_ms milliseconds has passaed and its ok to run your function
  * @retval false Interval_ms milliseconds has not passaed, dont run your function.
  */
  inline bool run_interval(std::chrono::time_point<std::chrono::system_clock>& timer, const unsigned long interval_ms) {
    using namespace std::chrono;
    if (duration_cast<milliseconds>(system_clock::now() - timer).count() > interval_ms) {
      timer = system_clock::now();
      return true;
    }
    return false;
  }

}