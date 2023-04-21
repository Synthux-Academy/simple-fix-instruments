#pragma once

#define C4 261.63f
#define D4 293.66f
#define E4 329.63f
#define F4 349.23f
#define G4 392.00f
#define A4 440.00f
#define B4 493.88f
#define C5 523.25f

class SimpleMelody {
public:
  SimpleMelody() = default;
  ~SimpleMelody() = default;

  void add_note(float note) {
    _notes[_note_count] = note;
    _note_count ++;
  }

  float note() {
    auto note = _notes[_note_index];
    _note_index ++;
    if (_note_index >= _note_count) {
      _note_index = 0;
    }
    return note;
  }

private:
  float _notes[1024];
  int _note_count = 0;
  int _note_index = 0;
};
