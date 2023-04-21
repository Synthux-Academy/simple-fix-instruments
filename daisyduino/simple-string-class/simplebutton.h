#pragma once

#include "DaisyDuino.h"

class SimpleButton {
public:
  SimpleButton() = default;
  ~SimpleButton() = default;

  void setup() {
    pinMode(kPin, INPUT_PULLDOWN);
  }

  bool is_pressed() {
    if (--_read_countdown == 0) {
      _read_countdown = kReadInterval;
      auto is_pushed = digitalRead(kPin);
      _is_pressed = is_pushed && !_was_pushed;
      _was_pushed = is_pushed;
    }
    return _is_pressed;
  }

private:
  const int kPin = 2;
  bool _is_pressed = false;
  bool _was_pushed = false;
  const uint32_t kReadInterval = 200;
  uint32_t _read_countdown = kReadInterval;
};
