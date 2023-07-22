#pragma once

namespace synthux {

  // Maps note number to the frequency.
  // The scale is being generated using superparticular ratios: 
  // 3/2 aka fifth and 5/4 aka major third.
  // If the frequency exceeds 1500Hz, it's getting wrapped 
  // by an octave down untill it fits the limit.
  template<uint8_t scale_size>
  class Scale {
  public:
    Scale() {
        float freq = 36.f;
        float third;
        size_t i = 0;
        while (i < scale_size) {
            while (freq > kMaxFreq) freq *= .5f;
            _scale[i++] = freq;
            third = freq * 1.25f;
            while (third > kMaxFreq) third *= .5f;
            _scale[i++] = third;
            freq *= 1.5;
        }
    }

    float freqAt(uint8_t idx) {
      return _scale[idx];
    }

  private:
      static constexpr float kMaxFreq = 1500;
      float _scale[scale_size];
  };
};

