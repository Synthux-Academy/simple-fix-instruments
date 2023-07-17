#pragma once

namespace synthux {

  typedef void(*NoteCallback)(uint8_t num);

  class Arp {
  public:
    enum Direction {
      up,
      down
    };

    Arp() {
      Clear();
    }

    void NoteOn(uint8_t num) {
      uint8_t slot = 1;
      while (slot < kBufferSize + 1) {
        if (_notes[slot].next == kUnlinked) break;
        slot++;
      }
      if (slot == kBufferSize + 1) {
        slot = _input_order[0];
        _RemoveNote(slot);
      }

      auto idx = _bottom_idx;
      while (_notes[idx].num < num) idx = _notes[idx].next;

      if (idx == _bottom_idx) _bottom_idx = slot;
      else if (idx == 0) _top_idx = slot;

      _notes[slot].num = num;
      _notes[slot].next = idx;
      _notes[slot].prev = _notes[idx].prev;
      _notes[_notes[idx].prev].next = slot;
      _notes[idx].prev = slot;
      _input_order[_size - 1] = slot;
      _size ++;
    }

    void NoteOff(uint8_t num) {
      uint8_t idx = 0;
      for (uint8_t i = 0; i < _size; i++)
          if (_notes[i].num == num) {
              idx = i;
              break;
          }

      if (idx != 0) _RemoveNote(idx);
    }

    void SetOnNoteOn(NoteCallback on_note_on) {
      _on_note_on = on_note_on;
    }

    void SetOnNoteOff(NoteCallback on_note_off) {
        _on_note_off = on_note_off;
    }

    void SetDirection(Direction direction) {
      _direction = direction;
    }

    void SetRandomize(float rand) {
      _rand = rand;
    }

    void Trigger() {
      if (_size <= 1) return;

      if (_counter == 2 && _current_idx > 0) {
        _on_note_off(_notes[_current_idx].num);
      }
      
      if (_counter++ < 6) return;
      
      _counter = 0;
      uint8_t note_idx;
      switch (_direction) {
        case up: 
          note_idx = _notes[_current_idx].next; 
          if (note_idx == 0) note_idx = _notes[note_idx].next; 
          break;

        case down: 
          note_idx = _notes[_current_idx].prev; 
          if (note_idx == 0) note_idx = _notes[note_idx].prev; 
          break;
      }

      if (_rand > 0.05 && _rand < 0.95) {
       auto rnd = static_cast<float>(random(0, 100)) / 100.f;
       if (rnd <= _rand) {
         note_idx = random(1, _size - 1);
       }
      }

      _current_idx = note_idx;

      _on_note_on(_notes[note_idx].num);
    }

    void Clear() {
      memset(_input_order, 0, sizeof(uint8_t) * kBufferSize);
      _notes[0].num = kSentinel;
      _notes[0].prev = 0;
      _notes[0].next = 0;
      _size = 1;
      for (uint8_t i = _size; i < kBufferSize + 1; i++) {
          _notes[i].num = 0;
          _notes[i].prev = kUnlinked;
          _notes[i].next = kUnlinked;
      }
    }

  private:
    void _RemoveNote(uint8_t idx) {
      _notes[_notes[idx].prev].next = _notes[idx].next;
      _notes[_notes[idx].next].prev = _notes[idx].prev;
      if (idx == _top_idx) _top_idx = _notes[idx].prev;
      else if (idx == _bottom_idx) _bottom_idx = _notes[idx].next;
      _notes[idx].num = 0;
      _notes[idx].next = kUnlinked;
      _notes[idx].prev = kUnlinked;

      for (uint8_t i = 0; i < _size; i++) {
          if (_input_order[i] == idx) {
              while (i < _size) {
                  _input_order[i] = _input_order[i + 1];
                  i++;
              }
          }
      }
      _size--;
    }

    struct Note {
      uint8_t next;
      uint8_t prev;
      uint8_t num;
    };

    static const uint8_t kSentinel = 128;
    static const uint8_t kUnlinked = 128;
    static const uint8_t kBufferSize = 15;

    NoteCallback _on_note_on  = nullptr;
    NoteCallback _on_note_off = nullptr;

    Note _notes[kBufferSize + 1];
    uint8_t _input_order[kBufferSize];

    Direction _direction = Direction::up;
    float _rand = 0;
    
    uint8_t _top_idx      = 0;
    uint8_t _bottom_idx   = 0;
    uint8_t _current_idx  = 0;
    uint8_t _counter      = 0;
    uint8_t _size         = 0;
  };
};
