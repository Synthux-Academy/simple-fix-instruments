#ifndef SYNTHUX_FLTR_H
#define SYNTHUX_FLTR_H

#include "DaisyDuino.h"

class Filter {
public:
  void Init(float sampleRate) {
    _flt.Init(sampleRate);
  }

  void SetTimbre(float timbre) {
    auto fltFreq = fmap(timbre, 60, 15000);
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

#endif
