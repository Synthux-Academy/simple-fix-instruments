#include "WSerial.h"
#pragma once
#include "Adafruit_MPR121.h"
#include <Wire.h>
#include <array>

namespace synthux {
  class Terminal {
    public:
      static constexpr uint16_t kNotesCount = 8;

      Terminal():
        _state { 0 },
        _latch { false },
        _on_note_on { nullptr },
        _on_note_off { nullptr }
        {
          _hold.fill(false);
        }

      void Init() {
        if (!_cap.begin(0x5A)) {
          Serial.println("MPR121 not found, check wiring?");
          while (1) {
            Serial.println("PLEASE CONNECT MPR121 TO CONTINUE TESTING");
            delay(200);
          }
        }
      }

      // Register note on callback
      void SetOnNoteOn(void(*on_note_on)(uint8_t num, uint8_t vel)) {
        _on_note_on = on_note_on;
      }

      // Register note off callback
      void SetOnNoteOff(void(*on_note_off)(uint8_t num)) {
        _on_note_off = on_note_off;
      }

      void SetOnScaleSelect(void(*on_scale_select)(uint8_t num)) {
        _on_scale_select = on_scale_select;
      }

      bool IsLatched() {
        return _latch;
      }

      void Process() {
          uint16_t p;
          bool is_touched;
          bool was_touched;
          auto state = _cap.touched();

          //Latch on/off
          //Pin 8
          is_touched = state & kLatchPin;
          was_touched = _state & kLatchPin;
          if (is_touched && !was_touched) {
            _latch = !_latch;
            if (!_latch) {
              for (uint16_t i = 0; i < _hold.size(); i++) {
                if (_hold[i] && !(state & 1 << i)) _on_note_off(i);
              }
              _hold.fill(false);
            }
          }

          //Notes on/off
          //Pins 0 - 7
          for (uint16_t i = 0; i < kNotesCount; i++) {
            p = 1 << i;
            is_touched = state & p;
            was_touched = _state & p;
            if (is_touched && !was_touched) {
              if (_latch && _hold[i]) _SetOff(i);
              else _SetOn(i);
            }
            else if (!is_touched && was_touched) {
              if (!_latch) _SetOff(i);
            }
          }

          //Scale select
          //Pins 9, 10, 11
          for (uint16_t i = 9; i < 12; i++) {
            p = 1 << i;
            is_touched = state & p;
            was_touched = _state & p;
            if (is_touched && !was_touched) {
              _on_scale_select(i - 9);
            }
          }

          _state = state;
      }

    private:
      void _SetOn(uint16_t i) {
        _on_note_on(i, 127);
        _hold[i] = true;
      }

      void _SetOff(uint16_t i) {
        _on_note_off(i);
        _hold[i] = false;
      }

      void(*_on_note_on)(uint8_t num, uint8_t vel);
      void(*_on_note_off)(uint8_t num);
      void(*_on_scale_select)(uint8_t index);

      static constexpr uint16_t kLatchPin = 1 << 8;

      Adafruit_MPR121 _cap;
      uint16_t _state { 0 };
      std::array<bool, kNotesCount> _hold;
      bool _latch { false };
  };
};
