#pragma once

namespace synthux {

  class Scale {
  public:
    Scale() {
      float freq = 36.f;
      float third;
      float half;
      size_t i = 0;
      while (i < kScaleSize) {
        third = freq * 1.25f;
        if (third < kMaxFreq) _scale[i++] = third;
        if (freq < kMaxFreq) _scale[i++] = freq;
        half = freq * .5f;
        if (half < kMaxFreq) _scale[i++] = half;
        freq *= 1.5f;
      }
    }

    float freqAt(size_t idx) {
      return _scale[idx];
    }

  private:
      static const float kMaxFreq = 1500;
      static const int kScaleSize = 30;
      float _scale[kScaleSize];
  }
  
};
