#pragma once

namespace synthux {
  
  // Mimiks notes input from a keyboard 
  // or an other human interface device.
  template<uint8_t notes_count, uint8_t scale_size>
  class Terminal {
    public:

      // Register note on callback
      void SetOnNoteOn(void(*on_note_on)(uint8_t num, uint8_t vel)) {
        _on_note_on = on_note_on;
      }

      // Register note off callback
      void SetOnNoteOff(void(*on_note_off)(uint8_t num)) {
        _on_note_off = on_note_off;
      }

      // Assuming scale_size is larger than the notes count,
      // each "voice" gets particular range of possible note 
      // numbers. The size of the range depends on how big 
      // the scale size is and the "voice" number (the lower the
      // number - the shorter the range). 
      // For instance for scale_size of 36 and notes_count of 12, 
      // the ranges are:
      // voice 1: [0, 1, 2]
      // voice 2: [0, 1, 2, 3, 4, 5]
      // ........
      // voice 11: [0, 1, 2, 3, 4, 5, ....., 29, 30, 31, 32]
      // voice 12: [0, 1, 2, 3, 4, 5, ....., 29, 30, 31, 32, 33, 34, 35]
      //
      // and the note numbers for offset = 0.6:
      //
      // voice  : 1 2 3 4 5 6  7  8  9  10 11 12
      // number : 2 4 5 7 9 11 13 14 16 18 20 22

      void SetOffset(float offset) {
        // Prevent generating if there was no change in offset
        auto note = static_cast<uint8_t>((scale_size - 1) * offset);
        if (note == _notes[notes_count - 1]) return;

        // Step for note number range
        uint8_t step = scale_size / notes_count;
        uint8_t idx;
        for (uint8_t i = notes_count; i > 0; i--) {
            idx = i-1;
            // Set voice to off, mimiking release of the key.
            _on_note_off(_notes[idx]);
            
            // Generate note number for the voice at given offset
            _notes[idx] = static_cast<uint8_t>((scale_size - step * (notes_count - i) - 1) * offset);
            
            // Set the note on, randomizing velocity
            _on_note_on(_notes[idx], random(90, 127));
        }
      }

    private:
      uint8_t _notes[notes_count];
      void(*_on_note_on)(uint8_t num, uint8_t vel) = nullptr;
      void(*_on_note_off)(uint8_t num) = nullptr;
  };
};
