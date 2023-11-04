#pragma once

#include "buf.h"
#include "win.h"
#include <array>

namespace synthux {

template<size_t win_slope = 480>
class Looper {
  public:
    Looper():
    _delta           { 1.f },
    _loop_start      { 0 },
    _loop_length     { 0 }
    {}

    void Init(float **buf, size_t length) {
        _buffer.Init(buf, length, win_slope);
        _loop_length = length;
        _Activate(0);
    }

    void SetRecording(bool is_rec_on) {
        _buffer.SetRecording(is_rec_on);
    }

    void SetSpeed(const float value) {
        _delta = value;
    }

    void SetLoop(const float loop_start, const float loop_length) {
      auto new_start = static_cast<size_t>(loop_start * _buffer.Length());
      if (abs(static_cast<int32_t>(new_start) - static_cast<int32_t>(_loop_start)) > 500) _loop_start = new_start;
      
      auto new_length = static_cast<size_t>(loop_length * _buffer.Length());
      auto norm_length = (new_length / win_slope) * win_slope;
      if (new_length - norm_length > win_slope / 2) norm_length += win_slope;
      if (abs(static_cast<int32_t>(norm_length) - static_cast<int32_t>(_loop_length)) > win_slope) {
        _loop_length = std::max(2 * win_slope, norm_length);
      }
    }
  
    void Process(float& in0, float& in1, float& out0, float& out1) {
        _buffer.Write(in0, in1);
        if (_buffer.IsRecording()) {
          out0 = in0;
          out1 = in1;
        };

        for (auto& w: _wins)
            if (w.IsHalf()) _Activate(w.PlayHead());

        out0 = 0.f;
        out1 = 0.f;
        for (auto& w: _wins) {
            if (!w.IsActive()) continue;
            
            auto w_out0 = 0.f;
            auto w_out1 = 0.f;
            w.Process(_buffer, w_out0, w_out1);
            out0 += w_out0;
            out1 += w_out1;
        }
    }

private:
    void _Activate(float play_head) {
       for (auto& w: _wins) {
            if (!w.IsActive()) {
                w.Activate(play_head, _delta, _loop_start, _loop_length);
                break;
            }
        }
    }

    static const size_t kMinLoopLength = 2 * win_slope;

    Buffer _buffer;
    std::array<Window<win_slope>, 2> _wins;

    float _delta;
    size_t _loop_start;
    size_t _loop_length;
};
};
