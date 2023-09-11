#pragma once

namespace synthux {
  class Scale {
  public:
    Scale() {
        
    }

    float freqAt(uint8_t idx) {
      return _scale[idx];
    }

  private:
      float _scale[8] = { 146.83, 164.81, 185, 196, 246.94, 293.66, 329.63, 369.99 };
  };
};
