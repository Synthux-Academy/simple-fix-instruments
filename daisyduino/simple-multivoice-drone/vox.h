#include <cstdlib>
#pragma once

#include "DaisyDuino.h"

class Vox {
public:

static constexpr float kOscLowestFreq = 36;

void Init(float sampleRate, float oscHighestFreq) {
  // OSCILLATOR SETUP
  _osc.Init(sampleRate);
  _osc.SetWaveform(Oscillator::WAVE_SAW);
  _oscHighestFreq = oscHighestFreq;

  // LFO SETUP
  _lfo.Init(sampleRate);
  _lfo.SetFreq(random(50, 150) / 10.0);
  _lfo.SetWaveform(_lfo.WAVE_SIN);
}

void Read(int oscPitchPin, int oscFreqPin) {
  auto pitch = fmap(analogRead(oscPitchPin) / 1023.0, 0, 500); 
  _oscFreq = fmap(analogRead(oscFreqPin) / 1023.0, kOscLowestFreq + pitch, _oscHighestFreq + pitch);
  _lfo.SetAmp(0.005 * analogRead(oscFreqPin) / 1023.0);
}

float Process() {
    _osc.SetFreq(_oscFreq * (1.0 + _lfo.Process()));
    return _osc.Process();
}

private:
  Oscillator _osc;
  Oscillator _lfo;

  float _oscFreq;
  float _oscHighestFreq;
};
