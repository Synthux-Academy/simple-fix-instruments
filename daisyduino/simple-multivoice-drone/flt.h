#pragma once

#include "DaisyDuino.h"

class Filter {
public:
  void Init(float sampleRate) {
    _flt.Init(sampleRate);
  }

  void Read(int pin) {
    auto timbre = analogRead(pin) / 1023.0;
    auto fltFreq = fmap(timbre, 60, 10000);
    auto fltRes = fmap(1 - timbre, 0, 0.5);
    _flt.SetFreq(fltFreq);
    _flt.SetRes(fltRes); 
  }

  float Process(const float in) {
    return _flt.Process(in);
  }

private:
  daisysp::MoogLadder _flt;
};
