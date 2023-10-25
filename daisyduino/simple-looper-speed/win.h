#pragma once

namespace synthux {

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

    float LoopStart() { return _loop_start; }

    float LoopLength() { return _loop_length; }

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
        
        std::cout << "I" << _iterator << "P" << int_ph + _loop_start << " ";

        _Advance();

        return output;
    }
  
private:
    float _Attenuation() {
        if (_iterator < kHalf) return static_cast<float>(_iterator) * kSlope;
        else return 1 - static_cast<float>(_iterator - kHalf) * kSlope;
    }
  
    void _Advance() {
        _play_head += _delta;
        if (_play_head < 0) _play_head += _loop_length;
        else if (_play_head >= _loop_length) _play_head -= _loop_length;
        if (++_iterator == kSize) _is_active = false;
    }

    const size_t kHalf { win_slope };
    const size_t kSize { 2 * win_slope };
    const float kSlope { 1.f / static_cast<float>(win_slope - 1) };

    float _play_head;
    float _delta;
    size_t _loop_start;
    size_t _loop_length;

    size_t _iterator;
    bool _is_active;
};

};
