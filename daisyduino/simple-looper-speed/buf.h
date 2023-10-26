#pragma once

namespace synthux {

class Buffer {
  public:
    void Init(float *buf, size_t length, size_t env_slope = 480) {
      _buffer = buf;
      _buffer_length = length;
      _env_slope = env_slope;
      // Reset buffer contents to zero
      memset(_buffer, 0, sizeof(float) * length);
    }

    size_t Length() {
      return _buffer_length;
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
  
    float Read(size_t frame) {
      if (frame >= _buffer_length) frame -= _buffer_length;
      return _buffer[frame];
    }

    void Write(float in) {
      // Calculate iterator position on the record level ramp.
      if ((_rec_env_pos_inc > 0 && _rec_env_pos < _env_slope)
       || (_rec_env_pos_inc < 0 && _rec_env_pos > 0)) {
          _rec_env_pos += _rec_env_pos_inc;
      }
      // If we're in the middle of the ramp - record to the buffer.
      if (_rec_env_pos > 0) {
        // Calculate fade in/out
        float rec_attenuation = static_cast<float>(_rec_env_pos - 1) / static_cast<float>(_env_slope - 1);
        _buffer[_rec_head] = in * rec_attenuation + _buffer[_rec_head] * (1.f - rec_attenuation);
        if (++_rec_head == _buffer_length) _rec_head = 0;
      }
    }

  private:
    float* _buffer;
    
    size_t  _buffer_length      { 0 };
    size_t  _loop_length        { 0 };
    size_t  _loop_start         { 0 };

    float   _play_head          { 0 };
    size_t  _rec_head           { 0 };

    size_t  _env_slope          { 480 };
    size_t  _rec_env_pos        { 0 };
    int32_t _rec_env_pos_inc    { 0 };
    bool    _is_empty           { true };
    bool    _is_loop_set        { false };
};
};
