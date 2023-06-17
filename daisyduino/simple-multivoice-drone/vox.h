#ifndef SYNTHUX_VOX_H
#define SYNTHUX_VOX_H

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
  auto oscFreqOffset = fmap(analogRead(oscPitchPin) / 1023.f, 0, 500);
  auto oscFreqMin =  kOscLowestFreq + oscFreqOffset;
  auto oscFreqMax = _oscHighestFreq + oscFreqOffset;
  _oscFreq = fmap(analogRead(oscFreqPin) / 1023.f, oscFreqMin, oscFreqMax);
  auto lfoAmp = fmap(analogRead(oscFreqPin) / 1023.f, 0.f, 0.005);
  _lfo.SetAmp(lfoAmp);
}

float Process() {
    _osc.SetFreq(_oscFreq * (1.f + _lfo.Process()));
    return _osc.Process();
}

private:
  Oscillator _osc;
  Oscillator _lfo;
  float _oscFreq;
  float _oscHighestFreq;
};

#endif