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

  _env.Init(sampleRate);
  _env.SetTime(ADSR_SEG_ATTACK, 0);
  _env.SetTime(ADSR_SEG_RELEASE, .05);
}

void Read(int oscPitchPin, int oscFreqPin) {
  auto oscFreqOffset = fmap(analogRead(oscPitchPin) / 1023.f, 0, 500);
  auto oscFreqMin =  kOscLowestFreq + oscFreqOffset;
  auto oscFreqMax = _oscHighestFreq + oscFreqOffset;
  _oscFreq = fmap(analogRead(oscFreqPin) / 1023.f, oscFreqMin, oscFreqMax);
  auto lfoAmp = fmap(analogRead(oscFreqPin) / 1023.f, 0.f, 0.005);
  _lfo.SetAmp(lfoAmp);
}

void Trigger() {
  _gate = open;
}

float Process() {
    auto env_amp = _env.Process(_gate == open);
    if (!_env.IsRunning()) return 0;
    _gate = closed;
    _osc.SetAmp(env_amp);
    _osc.SetFreq(_oscFreq * (1.f + _lfo.Process()));
    return _osc.Process();
}

private:
  enum Gate {
    closed,
    open
  };
  Gate _gate = closed;

  Oscillator _osc;
  Oscillator _lfo;
  Adsr _env;
  float _oscFreq;
  float _oscHighestFreq;
};

#endif