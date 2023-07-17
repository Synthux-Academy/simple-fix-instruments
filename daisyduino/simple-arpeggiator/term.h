#pragma once

namespace synthux {
  
  class Terminal {
    public:
      void SetOnNoteOn(void(*on_note_on)(int32_t num)) {
        _on_note_on = on_note_on;
      }

      void SetOnNoteOff(void(*on_note_off)(int32_t num)) {
        _on_note_off = on_note_off;
      }

      void Read(float inputPin) {
        auto note = static_cast<int32_t>(36 * inputPin);
        if (note == _notes[11]) return;

        for (size_t i = 0; i < 12; i++) {
          _on_note_off(_notes[i]);
          _notes[i] = static_cast<int32_t>(3 * (1 + i) * inputPin);
          _on_note_on(_notes[i]);
        }
      }

    private:
      int32_t _notes[12];
      void(*_on_note_on)(int32_t num);
      void(*_on_note_off)(int32_t num);
  };
};
