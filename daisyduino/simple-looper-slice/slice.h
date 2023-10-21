#pragma once

namespace synthux {

class Slice {
public:
  Slice(): 
  _init_pos        { 0 },
  _is_active       { false },
  _play_head       { 0 },
  _play_head_delta { 1 }
  {}

  float Activate(float position) {
    _is_active = true;
  }

  float PlayHead() {
    return _play_head;
  }

  bool isMiddle() {
    return _play_head <= kFadeLength && _play_head + 1 > kFadeLength;
  }

  float Process(float* buffer, size_t buffer_length) {
    // Playback from the buffer
      float attenuation = 1;
      float output = 0;
      //Calculate fade in/out
      if (_play_head < kFadeLength) {
        attenuation = static_cast<float>(_play_head) / static_cast<float>(kFadeLength);
      }
      else {
        attenuation = static_cast<float>(kLength - _play_head) / static_cast<float>(kFadeLength);
      }
      
      // Read from the buffer
      auto _int_ph = static_cast<uint32_t>(_play_head);
      auto _frac_ph = _play_head - _int_ph;
      auto _next_ph = (_int_ph + 1);
      if (_next_ph < 0 || _next_ph > kLength) _is_active = false;;
      output = buffer[_int_ph] + _frac_ph * (buffer[_next_ph] - buffer[_int_ph]);
      _play_head += _play_head_delta;
      if (_play_head < 0 || _play_head > kLength) _is_active = false;
      
      return output * attenuation;
  }

private:
  static constexpr float kFadeLength = 480;
  static constexpr float kLength = 2 * kFadeLength;

  float _init_pos;
  bool _is_active;
  float _play_head;
  float _play_head_delta;
};

};
