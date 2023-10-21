#pragma once

namespace synthux {

class Window {
public:
  static constexpr size_t kHalf { 480 }; // 10ms @ 48K
  static constexpr size_t kSize { 960 };  // 2 x Slope
  static constexpr float kSlope { 1 / static_cast<float>(kHalf) };

  Window():
  _play_head { 0 },
  _ph_delta  { 0 },
  _loop_start { 0 },
  _loop_length { 0 },
  _iterator  { 0 },
  _is_active { false }
  {}

  void Activate(float start, float delta, size_t loop_start, size_t loop_length) {
    _play_head = start;
    _delta = delta;
    _loop_start = loop_start;
    _loop_length = loop_length;
    _is_active = true;
  }

  bool IsActive() {
    return _is_active;
  }

  bool IsBeforeMiddle() {
    return _iterator == kHalf - 1;
  }

  float NextPosition() {
    return _play_head + _play_head_delta;
  }

  float Process(Buffer& buf) {
    auto int_ph = static_cast<size_t>(_play_head);
    auto frac_ph = _play_head - int_ph;
    auto next_ph = int_ph + 1;

    if (next_ph < 0) next_ph += _loop_length;
    if (next_ph >= _loop_length) next_ph -= _loop_length;
    
    auto sample_a = buf.Read(int_ph + _loop_start);
    auto sample_b = buf.Read(next_ph + _loop_start);
    output = (sample_a + frac_ph * (sample_b - sample_a)) * _Attenuation();
    
    _play_head += _delta;
    if (_play_head < 0) _play_head += _loop_length;
    else if (_play_head >= _loop_length) _play_head -= _loop_length;
    
    if (++_iterator == kSize) _is_active = false;

    return output;
  }
  
private:
  float _Attenuation() {
    return (_iterator <= kHalf ? _iterator : (kSize - _iterator)) * kSlope;
  }
  
  float _play_head;
  float _ph_delta;
  size_t _loop_start;
  size_t _loop_length;

  size_t _iterator;
  bool _is_active;
};

};