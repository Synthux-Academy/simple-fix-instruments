#ifndef SYNTHUX_LOOPER_H
#define SYNTHUX_LOOPER_H

#include "DaisyDuino.h"

namespace synthux {

class Looper {
  public:

    static const long kBufferLengthSec = 5;
    static const long kSampleRate = 48000;
    static const long kBufferLenghtSamples = kBufferLengthSec * kSampleRate;

    void Init(float sample_rate, float *buf, long length) {
      _buffer = buf;
      _buffer_length = length;
      memset(_buffer, 0, sizeof(float) * _buffer_length);

      _pitch.Init(sample_rate);
    }

    void SetRecording(bool is_rec_on) {
        if (!_rec_on && is_rec_on) {
            _rec_head = (_loop_start + _play_head) % _buffer_length;
        }
        _rec_on = is_rec_on;
    }

    void SetLoop(const float loop_start, const float loop_length) {
      _next_loop_start = static_cast<long>(loop_start * (_buffer_length - 1));
      if (_loop_start == -1) _loop_start = _next_loop_start;
      _loop_length = max(static_cast<long>(480), static_cast<long>(loop_length * _buffer_length));
    }

    void SetPitch(const float loop_start_mod) {
      int pitch = 0;
      _pitch_on = false;
      if (loop_start_mod < 0.45 || loop_start_mod > 0.55) {
        pitch = 12.0 * (loop_start_mod - 0.5);
        _pitch_on = true;
      }
      _pitch.SetTransposition(pitch);
    }

    float Process(float in) {
      if (_rec_on) {
        _is_empty = false;
        auto rec_pos = _rec_head % _buffer_length;
        _buffer[rec_pos] = in;
        _rec_head ++;
      }

      float att = 1;
      float output = 0;
      if (!_is_empty) {
        auto play_pos = (_loop_start + _play_head) % _buffer_length;
        output = _buffer[play_pos];

        if (_play_head < 192) {
          att = static_cast<float>(_play_head) / 192.f;
        }
        else if (_play_head >= _loop_length - 192) {
          att = static_cast<float>(_loop_length - _play_head) / 192.f;
        }

        _play_head ++;
        if (_play_head >= _loop_length) {
          _loop_start = _next_loop_start;
          _play_head = 0;
        }
      }
      
      return (_pitch_on ? _pitch.Process(output) : output) * att;
    }

  private:
    PitchShifter _pitch;

    float* _buffer;
    
    long _buffer_length   = 0;
    long _loop_start      = -1;
    long _next_loop_start = 0;
    long _loop_length     = 0;

    long _play_head       = 0;
    long _rec_head        = 0;

    bool _rec_on          = false;
    bool _is_empty        = true;
    bool _pitch_on        = false;
};
};

#endif
