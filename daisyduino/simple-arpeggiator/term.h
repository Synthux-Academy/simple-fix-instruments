#pragma once

#include <array>

static const int kVoicesCount = 12;

namespace synthux {
  class Terminal {
    public:
      void Init() {
        float freq = 36.f;
        float third;
        float half;
        size_t i = 0;
        while (i < _scale.size()) {
          third = freq * 1.25f;
          if (third < kMaxFreq) _scale[i++] = third;
          if (freq < kMaxFreq) _scale[i++] = freq;
          half = freq * .5f;
          if (half < kMaxFreq) _scale[i++] = half;
          freq *= 1.5f;
        }
      }

      void Read(float inputPin) {
        size_t index;
        for (size_t i = 0; i < kVoicesCount; i++) {
          index = static_cast<size_t>(3 * (1 + i) * inputPin);
          _chord[i] = _scale[index];
        }
      }

      std::array<float, kVoicesCount>& Chord() {
        return _chord;
      }

    private:
        const float kMaxFreq = 1500;
        std::array<float, kVoicesCount> _chord;
        std::array<float, 30> _scale;
  }
}
