
#include "simple-daisy.h"
#include "looper.h"

// Setup pins
static const int record_pin      = D(S35);
static const int loop_start_pin  = A(S33);
static const int loop_length_pin = A(S30);
static const int loop_speed_pin  = A(S37);

static const float kKnobMax = 1023;

// Allocate buffer in SDRAM 
static const uint32_t kBufferLengthSec = 5;
static const uint32_t kSampleRate = 48000;
static const size_t kBufferLenghtSamples = kBufferLengthSec * kSampleRate;
static float DSY_SDRAM_BSS buffer0[kBufferLenghtSamples];
static float DSY_SDRAM_BSS buffer1[kBufferLenghtSamples];
static float* buffer[2] = { buffer0, buffer1 };

static synthux::Looper looper;
static Oscillator mod;

float mod_val = 0;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) looper.Process(in[0][i], in[1][i], out[0][i], out[1][i]);
}

void setup() {
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  float sample_rate = DAISY.get_samplerate();

  mod.Init(sample_rate);
  mod.SetFreq(100);
  mod.SetWaveform(Oscillator::WAVE_SQUARE);

  // Setup looper
  looper.Init(buffer, kBufferLenghtSamples);

  // Setup pins
  pinMode(record_pin, INPUT_PULLUP);

  DAISY.begin(AudioCallback);
}

void loop() {
  auto raw_loop_speed = analogRead(loop_speed_pin); 
  auto loop_speed = 0.f;
  if (raw_loop_speed < 500 || raw_loop_speed > 524) {
    loop_speed = fmap(raw_loop_speed / kKnobMax, -1.f, 1.f);
  }
  looper.SetSpeed(1.f + loop_speed);

  auto loop_start = fmap(analogRead(loop_start_pin) / kKnobMax, 0.f, 1.f);
  auto loop_length = fmap(analogRead(loop_length_pin) / kKnobMax, 0.f, 1.f, Mapping::EXP);
  looper.SetLoop(loop_start, loop_length);

  // Toggle record
  auto record_on = !digitalRead(record_pin);
  looper.SetRecording(record_on);

  // auto mod_freq = fmap(analogRead(loop_speed_pin) / kKnobMax, 5.f, 100.0f);
  // mod.SetFreq(mod_freq);

  // // Set pitch
  // mod_val = fmap(analogRead(mod_pin) / kKnobMax, 0.f, 1.0f);

  delay(4);
}
