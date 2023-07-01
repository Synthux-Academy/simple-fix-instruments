
#include "simple-daisy.h"
#include "looper.h"

// Setup pins
static const int record_pin      = D(S30);
static const int loop_start_pin  = A(S31);
static const int loop_length_pin = A(S32);
static const int pitch_pin       = A(S33);

static const float kKnobMax = 1023;

// Allocate buffer in SDRAM 
static const uint32_t kBufferLengthSec = 5;
static const uint32_t kSampleRate = 48000;
static const size_t kBufferLenghtSamples = kBufferLengthSec * kSampleRate;
static float DSY_SDRAM_BSS buffer[kBufferLenghtSamples];

static synthux::Looper looper;
static PitchShifter pitch_shifter;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    auto looper_out = looper.Process(in[1][i]);
    out[0][i] = out[1][i] = pitch_shifter.Process(looper_out);
  }
}

void setup() {
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  float sample_rate = DAISY.get_samplerate();

  // Setup looper
  looper.Init(buffer, kBufferLenghtSamples);

  // Setup pitch shifter
  pitch_shifter.Init(sample_rate);

  // Setup pins
  pinMode(record_pin, INPUT);

  DAISY.begin(AudioCallback);
}

void loop() {
  // Set loop parameters
  auto loop_start = fmap(analogRead(loop_start_pin) / kKnobMax, 0.f, 1.f);
  auto loop_length = fmap(analogRead(loop_length_pin) / kKnobMax, 0.f, 1.f, Mapping::EXP);
  looper.SetLoop(loop_start, loop_length);

  // Toggle record
  auto record_on = digitalRead(record_pin);
  looper.SetRecording(record_on);

  // Set pitch
  auto pitch_val = fmap(analogRead(pitch_pin) / kKnobMax, 0.f, 1.f);
  set_pitch(pitch_val);
}

void set_pitch(float pitch_val) {
  int pitch = 0;
  // Allow some gap in the middle of the knob turn so 
  // it's easy to cacth zero position
  if (pitch_val < 0.45 || pitch_val > 0.55) {
    pitch = 12.0 * (pitch_val - 0.5);
  }
  pitch_shifter.SetTransposition(pitch);
}