#pragma once

namespace synthux {

  class Scale {
  public:
    Scale() {
      float freq = 36.f;
      float third;
      size_t i = 0;
      while (i < kScaleSize) {
        third = freq * 1.25f;
        if (third > kMaxFreq) third *= .5f;
        _scale[i++] = third;
        if (freq < kMaxFreq) _scale[i++] = freq;
        _scale[i++] = freq * .5f;
        freq *= 1.5f;
      }
    }

    float freqAt(size_t idx) {
      return _scale[idx];
    }

  private:
      static constexpr float kMaxFreq = 1500;
      static const int kScaleSize = 36;
      float _scale[kScaleSize];
  };
};
