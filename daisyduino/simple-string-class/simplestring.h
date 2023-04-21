/// Derived from https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/string/string.ino

#pragma once

#include "DaisyDuino.h"

class SimpleString {
public:
  SimpleString() = default;
  ~SimpleString() = default;

  void setup() {
    _string.Init(DAISY.get_samplerate());
    _string.SetDamping(.8f);
    _string.SetNonLinearity(.1f);
    _string.SetBrightness(.5f);
  }

  void pluck(float freq) {
    _string.SetFreq(freq);
    _should_excite = true;
  }

  void play(size_t size, float **out) {
    for (size_t i = 0; i < size; i++) {
        out[0][i] = out[1][i] = _string.Process(_should_excite);
        _should_excite = false;
    }
  }

private:
  StringOsc _string;
  bool _should_excite = false;
};
