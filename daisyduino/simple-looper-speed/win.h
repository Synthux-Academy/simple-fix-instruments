#pragma once
#include <array>

namespace synthux {

template<size_t length>
constexpr std::array<float, length> Slope() {
    std::array<float, length> slope { 0 };
    for (int i = 0; i < length; i++) slope[i] = static_cast<float>(i) / static_cast<float>(length - 1);
    return slope;
}

template<size_t win_slope>
class Window {
public:
    Window():
    _play_head    { 0 },
    _delta        { 0 },
    _loop_start   { 0 },
    _loop_length  { 0 },
    _iterator     { 0 },
    _is_active    { false }
    {}

    void Activate(float start, float delta, size_t loop_start, size_t loop_length) {
        _play_head = start;
        _delta = delta;
        _loop_start = loop_start;
        _loop_length = loop_length;
        _iterator = 0;
        _is_active = true;
    }

    bool IsActive() {
        return _is_active;
    }

    bool IsHalf() { return _iterator == kHalf; }

    float PlayHead() { return _play_head; }

    float Process(Buffer& buf) {
        auto int_ph = static_cast<size_t>(_play_head);
        auto frac_ph = _play_head - int_ph;
        auto next_ph = int_ph + 1;

        if (next_ph < 0) next_ph += _loop_length;
        if (next_ph >= _loop_length) next_ph -= _loop_length;

        auto sample_a = buf.Read(int_ph + _loop_start);
        auto sample_b = buf.Read(next_ph + _loop_start);
        auto att = _Attenuation();
        auto output = (sample_a + frac_ph * (sample_b - sample_a)) * att;

        _Advance();

        return output;
    }
  
private:
    float _Attenuation() {
      auto idx = (_iterator < kHalf) ? _iterator : kSize - _iterator - 1;
      return kSlope[idx];
    }
  
    void _Advance() {
        _play_head += _delta;
        if (_play_head < 0) _play_head += _loop_length;
        else if (_play_head >= _loop_length) _play_head -= _loop_length;
        if (++_iterator == kSize) _is_active = false;
    }

    static constexpr size_t kHalf { win_slope };
    static constexpr size_t kSize { 2 * win_slope };
    static constexpr std::array<float, win_slope> kSlope { Slope<win_slope>() };

    float _play_head;
    float _delta;
    size_t _loop_start;
    size_t _loop_length;

    size_t _iterator;
    bool _is_active;
};

};
