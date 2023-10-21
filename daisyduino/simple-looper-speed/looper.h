#pragma once

namespace synthux {

class Looper {
  public:
    void Init(float *buf, size_t length) {
      _buffer = buf;
      _buffer_length = length;
      // Reset buffer contents to zero
      memset(_buffer, 0, sizeof(float) * _buffer_length);
    }

    void SetRecording(bool is_rec_on) {
        //Initialize recording head position on start
        if (_rec_env_pos_inc <= 0 && is_rec_on) {
            _rec_head = (_loop_start + static_cast<int>(_play_head)) % _buffer_length; 
            _is_empty = false;
        }
        // When record switch changes state it effectively
        // sets ramp to rising/falling, providing a
        // fade in/out in the beginning and at the end of 
        // the recorded region.
        _rec_env_pos_inc = is_rec_on ? 1 : -1;
    }

    void SetSpeed(const float value) {
      _play_head_delta = value;
    }

    void SetLoop(const float loop_length) {
      // Set the start of the next loop
      _pending_loop_start = 0;

      // If the current loop start is not set yet, set it too
      if (!_is_loop_set) _loop_start = _pending_loop_start;

      // Set the length of the next loop
      _pending_loop_length = max(kMinLoopLength, static_cast<size_t>(loop_length * _buffer_length));

      //If the current loop length is not set yet, set it too
      if (!_is_loop_set) _loop_length = _pending_loop_length;
      _is_loop_set = true;
    }
  
    float Process(float in) {
      // Calculate iterator position on the record level ramp.
      if (_rec_env_pos_inc > 0 && _rec_env_pos < kFadeLength
       || _rec_env_pos_inc < 0 && _rec_env_pos > 0) {
          _rec_env_pos += _rec_env_pos_inc;
      }
      // If we're in the middle of the ramp - record to the buffer.
      if (_rec_env_pos > 0) {
        // Calculate fade in/out
        float rec_attenuation = static_cast<float>(_rec_env_pos) / static_cast<float>(kFadeLength);
        _buffer[_rec_head] = in * rec_attenuation + _buffer[_rec_head] * (1.f - rec_attenuation);
        if (++_rec_head == _buffer_length) _rec_head = 0;
      }
      
      if (_is_empty) {
        return 0;
      }

      // Playback from the buffer
      float attenuation = 1;
      float output = 0;
      //Calculate fade in/out
      if (_play_head < kFadeLength) {
        attenuation = static_cast<float>(_play_head) / static_cast<float>(kFadeLength);
      }
      else if (_play_head >= _loop_length - kFadeLength) {
        attenuation = static_cast<float>(_loop_length - _play_head) / static_cast<float>(kFadeLength);
      }
      
      // Read from the buffer
      auto _int_ph = static_cast<uint32_t>(_play_head);
      auto _frac_ph = _play_head - _int_ph;
      auto _next_ph = (_int_ph + 1);
      auto _loop_end = _loop_length - 1; 
      if (_next_ph < 0) _next_ph += _loop_end;
      if (_next_ph > _loop_end) _next_ph -= _loop_end;
      output = _buffer[_int_ph] + _frac_ph * (_buffer[_next_ph] - _buffer[_int_ph]);
      _play_head += _play_head_delta;
      if (_play_head < 0) {
        _play_head += _loop_end;
        _loop_length = _pending_loop_length;
      } 
      else if (_play_head > _loop_end) {
        _play_head -= _loop_end;
        _loop_length = _pending_loop_length;
      }
      
      return output * attenuation;
    }

  private:
    static const size_t kFadeLength = 600;
    static const size_t kMinLoopLength = 2 * kFadeLength;

    float* _buffer;

    size_t _buffer_length       = 0;
    size_t _loop_length         = 0;
    size_t _pending_loop_length = 0;
    size_t _loop_start          = 0;
    size_t _pending_loop_start  = 0;

    float _play_head = 0;
    float _play_head_delta = 0;
    size_t _rec_head  = 0;

    size_t _rec_env_pos      = 0;
    int32_t _rec_env_pos_inc = 0;
    bool _is_empty  = true;
    bool _is_loop_set = false;
};
};
