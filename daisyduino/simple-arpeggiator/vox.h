#pragma once

#include "DaisyDuino.h"

class Vox {
public:

static constexpr float kOscLowestFreq = 36;
static constexpr float kStep = 2187.f / 2048.f;


void Init(float sampleRate, int oscHighestFreq) {
  // OSCILLATOR SETUP
  _osc.Init(sampleRate);
  _osc.SetWaveform(Oscillator::WAVE_TRI);
  _oscHighestFreq = oscHighestFreq;

  // LFO SETUP
  _lfo.Init(sampleRate);
  _lfo.SetFreq(random(50, 150) / 10.0);
  _lfo.SetWaveform(_lfo.WAVE_SIN);

  //ENV SETUP
  _env.Init(sampleRate);
  _env.SetTime(ADSR_SEG_ATTACK, .05);
  _env.SetTime(ADSR_SEG_DECAY, .1);
  _env.SetTime(ADSR_SEG_RELEASE, .2);
}

void Trigger() {
  _gate = open;
}

void Read(int oscPitchPin, int oscFreqPin) {
  auto oscFreqOffset = fmap(analogRead(oscPitchPin) / 1023.f, 0, 500);
  auto oscFreqMin =  kOscLowestFreq + oscFreqOffset;
  auto oscFreqMax = _oscHighestFreq + oscFreqOffset;
  _oscFreq = fmap(analogRead(oscFreqPin) / 1023.f, oscFreqMin, oscFreqMax);
  // auto f = round((_oscFreq - oscFreqMin) / kStep);
  // _oscFreq = oscFreqMin + f * kStep;
  auto lfoAmp = fmap(analogRead(oscFreqPin) / 1023.f, 0.f, 0.005);
  _lfo.SetAmp(lfoAmp);
}

float Process() {
  
    auto amp = _env.Process(_gate == open);
    
    if (!_env.IsRunning()) {
      return 0;
    }
    if (_env.GetCurrentSegment() == ADSR_SEG_DECAY) {
      _gate = closed;
    }
    _osc.SetAmp(amp);
    _osc.SetFreq(_oscFreq * (1.f + _lfo.Process()));
    return _osc.Process();
}

private:
  enum Gate {
    closed,
    open
  };

  Oscillator _osc;
  Oscillator _lfo;
  Adsr _env;
  float _oscFreq;
  float _oscHighestFreq;

  Gate _gate = closed;
  long _gateTimeout = 0;
  long kGateMaxTimeout = 600;

};
