/// Derived from https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/string/string.ino

#include "DaisyDuino.h"

#include <algorithm>

static DaisyHardware seed;
static StringOsc str;

#define C4 261.63f
#define D4 293.66f
#define E4 329.63f
#define F4 349.23f
#define G4 392.00f
#define A4 440.00f
#define B4 493.88f
#define C5 523.25f

const int kArpegioLength = 3;
const float kArpeggio[kArpegioLength] = { C4, E4, G4 };
uint8_t arp_idx;

const float kTempo = 120;
const float kSecondsPerMinute = 60;
uint32_t trigger_interval;
uint32_t trigger_countdown;

static void AudioCallback(float **in, float **out, size_t size) {
  uint8_t excite_string;
  for (size_t i = 0; i < size; i++) {
    excite_string = 0;
    if (trigger_countdown == 0) {
      excite_string = 1;
      str.SetFreq(kArpeggio[arp_idx]);
      
      arp_idx = (arp_idx + 1) % kArpegioLength;
      trigger_countdown = trigger_interval;
    }
    trigger_countdown --;
    
    out[0][i] = out[1][i] = str.Process(excite_string);
  }
}

void setup() {
  // initialize seed hardware and daisysp modules
  float sample_rate;
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  sample_rate = DAISY.get_samplerate();

  float samples_per_beat = kSecondsPerMinute * sample_rate / kTempo;
  trigger_interval = samples_per_beat / 2; // 1/8th

  // Set up String algo
  str.Init(sample_rate);
  str.SetDamping(.8f);
  str.SetNonLinearity(.1f);
  str.SetBrightness(.5f);

  arp_idx = 0;

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {}