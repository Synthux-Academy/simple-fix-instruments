#pragma once

#include "DaisyDuino.h"

namespace synthux {

class Vox {
public:
void Init(float sample_rate) {
  // OSCILLATOR SETUP
  _osc.Init(sample_rate);
  _osc.SetWaveform(Oscillator::WAVE_TRI);

  // LFO SETUP
  _lfo.Init(sample_rate);
  _lfo.SetFreq(random(50, 150) / 10.0);
  _lfo.SetWaveform(_lfo.WAVE_POLYBLEP_TRI);
  _lfo.SetAmp(0.005);

  //ENV SETUP
  _env.Init(sample_rate);
  _env.SetTime(ADSR_SEG_ATTACK, .02);
  _env.SetTime(ADSR_SEG_RELEASE, .1);
}

void NoteOn(float freq) {
  _gate = open;
  _osc_freq = freq;
}

void NoteOff() {
  _gate = closed;
}

float Process() { 
    auto amp = _env.Process(_gate == open);
    if (!_env.IsRunning()) return 0;
    _osc.SetFreq(_osc_freq * (1.f + _lfo.Process()));
    _osc.SetAmp(amp);
    return _osc.Process();
}

private:
  enum Gate {
    closed,
    open
  };

  Gate _gate = closed;
  float _osc_freq;
  Oscillator _osc;
  Oscillator _lfo;
  Adsr _env;
};
};
