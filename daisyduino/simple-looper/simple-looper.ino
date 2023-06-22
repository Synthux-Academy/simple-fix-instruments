
#include "looper.h"

static float DSY_SDRAM_BSS buffer[synthux::Looper::kBufferLenghtSamples];

synthux::Looper lpr;

static const int kKnobResolution = 7;
static const float kKnobMax = pow(2, kKnobResolution);

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    out[0][i] = out[1][i] = lpr.Process(in[1][i]);
  }
}

void setup() {
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  float sample_rate = DAISY.get_samplerate();

  lpr.Init(sample_rate, buffer, synthux::Looper::kBufferLenghtSamples);
  
  analogReadResolution(kKnobResolution);
  pinMode(D15, INPUT);
  
  DAISY.begin(AudioCallback);
}

void loop() {
  auto loop_start = fmap(analogRead(A1) / kKnobMax, 0.f, 1.f);
  auto loop_length = fmap(analogRead(A2) / kKnobMax, 0.f, 1.f, Mapping::EXP);
  lpr.SetLoop(loop_start, loop_length);

  auto pitch = fmap(analogRead(A3) / kKnobMax, 0.f, 1.f);
  lpr.SetPitch(pitch);

  auto record_on = digitalRead(D15);
  lpr.SetRecording(record_on);

  delay(4);
}
