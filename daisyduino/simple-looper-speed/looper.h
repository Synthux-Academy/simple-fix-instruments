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
    _buffer_length   { 0 },
    _loop_start      { 0 },
    _loop_length     { 0 }
    {}

    void Init(float *buf, size_t length) {
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
        _loop_start = loop_start;
        _loop_length = loop_length;
    }
  
    float Process(float in) {
        _buffer.Write(in);
        float output = 0;
//        for (auto& w: _wins) {
//            if (w.IsHalf()) _Activate(w.PlayHead());
//        }

        for (auto& w: _wins) {
            if (w.IsActive()) {
                auto w_out = w.Process(_buffer);
                output += w_out;
                std::cout << w_out << " ";
            }
        }
        std::cout << " --> " << output << std::endl;
        return output;
    }

private:
    void _Activate(float start) {
//        for (auto& w: _wins) {
        for (int i = 0; i < _wins.size(); i++) {
            auto& w = _wins[i];
            if (!w.IsActive()) {
                w.Activate(start, _delta, _loop_start, _loop_length);
                std::cout << "A" << i << ">" << start << " ";
                break;
            }
        }
    }

    static const size_t kMinLoopLength = 2 * win_slope;

    Buffer _buffer;
    std::array<Window<win_slope>, 4> _wins;

    float _delta;
    size_t _buffer_length;
    size_t _loop_start;
    size_t _loop_length;
};
};
