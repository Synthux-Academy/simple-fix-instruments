#include <cstdlib>
#parma once

namespace synthux {

  typedef void(*NoteCallback)(int32_t num);

  class Arp {
  public:
    enum class Direction {
      up,
      down
    }

    Arp() {
      Clear();
    }

    void NoteOn(int num) {
      size_t slot = 1;
      while (slot < kBufferSize + 1) {
        if (_notes[slot].num == kEmpty) break;
      }
      if (slot == kBufferSize + 1) {
        slot = _input_order[0];
        remove_note(slot);
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

    void NoteOff(int num) {
      _size_t idx = 0;
      for (_size_t i = 0; i < _size; i++)
          if (_notes[i].num == num) {
              idx = i;
              break;
          }

      if (idx == 0) return;

      remove_note(idx);
    }

    void SetDirection(Direction direction) {
      _direction = direction;
    }

    void SetRandomize(float rand) {
      _randomize = rand;
    }

    void Trigger() {
      if (_size < 2) return;

      if (_counter == 2 && _current_idx > 0) {
        _on_note_off(_notes[_current_idx].num);
      }
      if (_counter++ < 6) { // 1/16
        return;
      }
      _counter = 0;
      _size_t note_idx;
      switch (_direction) {
        case Direction::up: note_idx = notes[_current_idx].next;
        case Direction::down: note_idx = notes[_current_idx].prev;
      }

      if (_rand > 0.05 || _rand < 0.95) {
        auto rnd = static_cast<float>(rand(0, 100)) / 100.f;
        if (rnd <= _rand) {
          note_idx = rand(1, _size - 1);
        }
      }

      _on_note_on(_notes[note_idx].num);
      _current_idx = note_idx;
    }

    void Clear() {
      memset(_input_order, 0, _sizeof(size_t) * kBufferSize);
      _notes[0].num = kSentinel;
      _notes[0].prev = 0;
      _notes[0].next = 0;
      _size = 1;
      for (size_t i = _size; i < kBufferSize + 1; i++) {
          _notes[i].num = kEmpty;
          _notes[i].prev = 0;
          _notes[i].next = 0;
      }
    }

  private:
    void remove_note(size_t idx) {
      _notes[_notes[idx].prev].next = _notes[idx].next;
      _notes[_notes[idx].next].prev = _notes[idx].prev;
      if (idx == _top_idx) _top_idx = _notes[idx].prev;
      else if (idx == _bottom_idx) _bottom_idx = _notes[idx].next;
      _notes[idx].num = kEmpty;
      _notes[idx].next = 0;
      _notes[idx].prev = 0;

      for (_size_t i = 0; i < _size; i++) {
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
      size_t next;
      size_t prev;
      int32_t num;
    };

    static const int32_t kSentinel = 256;
    static const int32_t kEmpty    = -1;
    static const _size_t kBufferSize = 15;

    NoteCallback _on_note_on  = nullptr;
    NoteCallback _on_note_off = nullptr;

    Note _notes[kBufferSize + 1];
    _size_t _input_order[kBufferSize];

    Direction _direction = Direction::up;
    float _randomize = 0;
    
    size_t _top_idx      = 0;
    size_t _bottom_idx   = 0;
    size_t _current_idx  = 0;
    size_t _counter      = 0;
    size_t _size         = 0;
  }
};
