#pragma once

#include "DaisyDuino.h"

namespace synthux {

class SimpleBD {
public:

  void Init(float sample_rate) {
    _noise.Init();
    _env.Init(sample_rate);
    _env.SetTime(ADSR_SEG_ATTACK, .0);
    _env.SetTime(ADSR_SEG_DECAY, .01);
    _env.SetTime(ADSR_SEG_RELEASE, .04);
    _env.SetSustainLevel(0.8);
    _osc.Init(sample_rate);
    _osc.SetWaveform(Oscillator::WAVE_SIN);
    _osc.SetFreq(50.0);
  }

  float Process(bool trigger) {
    if (trigger) {
      gate_counter = kGateCounterMax;
    }
    auto is_gate_open = gate_counter > 0;
    auto amp = _env.Process(is_gate_open);
    if (is_gate_open) gate_counter--;
    _noise.SetAmp(amp);
    _osc.SetAmp(amp);
    return _osc.Process() + _noise.Process() * 0.03;
  }

private:
  static constexpr uint32_t kGateCounterMax = 1920; //40ms @ 48K
  uint32_t gate_counter = 0;

  Adsr _env;
  WhiteNoise _noise;
  Oscillator _osc;
};

};
