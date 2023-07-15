#pragma once

static const int kVoicesCount = 12;

namespace synthux {
  
  class Terminal {
    public:
      void Read(float inputPin) {
        size_t index;
        for (size_t i = 0; i < kVoicesCount; i++) {
          index = static_cast<size_t>(3 * (1 + i) * inputPin);
          _chord[i] = _scale[index];
        }
      }

    private:
        
  }
}
