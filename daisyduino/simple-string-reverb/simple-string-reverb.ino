/// Derived from 
/// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/string/string.ino
/// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/reverbsc/reverbsc.ino

#include "DaisyDuino.h"

static DaisyHardware seed;
static StringOsc string;
static ReverbSc reverb;

//Reverb parameters
const float kReverbFeedback = 0.85;
const float kReverbLowPassFrequency = 18000; //Hz
const float kReverbWetMix = 0.33;

// Define note frequencies in Hz
// C4 is a "middle C" on the keyboard
#define C4 261.63f
#define D4 293.66f
#define E4 329.63f
#define F4 349.23f
#define G4 392.00f
#define A4 440.00f
#define B4 493.88f
#define C5 523.25f

// Define note sequence
const int kNotesCount = 3;
const float kNotes[kNotesCount] = { C4, E4, G4 };
int next_note_index = 0;

// Define button pin and state
const int kButtonPin = 2;
bool button_is_pushed = false;
bool button_was_pushed = false;

// A flag inidcating of whether the 
// string should be excited. We set
// it to 1 on button push and rest to 0
// after first buffer sample in Audio Callback 
float excite_string = 0;

static void AudioCallback(float **in, float **out, size_t size) {
  // Iterate through the number of samples in the buffer...
  for (size_t i = 0; i < size; i++) {
    // ... and fill both left and right channels 
    // of the output buffer with String Oscilator output...
    float string_out = string.Process(excite_string);
    // ... and reset the flag so the string gets excited 
    // only on the first sample of the buffer
    excite_string = 0;

    // Add Reverb ###################################
    // Read dry from string
    float dry_left = string_out;
    float dry_right = string_out;

    // Set placeholders for wet output
    float wet_left;
    float wet_right;
 
    // Process dry signal and send to wet playsholders
    reverb.Process(dry_left, dry_right, &wet_left, &wet_right);

    // Mix dry and wet and send to output
    out[0][i] = wet_left * kReverbWetMix + dry_left * (1 - kReverbWetMix);
    out[1][i] = wet_right * kReverbWetMix + dry_right * (1 - kReverbWetMix);
    // ##############################################
  }
}

void setup() {
  // Initialize Daisy Seed hardware
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  // Set up String Oscilator
  float sample_rate = DAISY.get_samplerate();
  string.Init(sample_rate);
  string.SetDamping(.8f);
  string.SetNonLinearity(.1f);
  string.SetBrightness(.5f);

  // Setup Reverb
  reverb.Init(sample_rate);
  reverb.SetFeedback(kReverbFeedback);
  reverb.SetLpFreq(kReverbLowPassFrequency);

  // Setup button
  // In this case button is connected to
  // Digital 3v3 (Pin 38), so we need
  // to pull pin down
  pinMode(kButtonPin, INPUT_PULLDOWN);

  // Start Audio Callback
  DAISY.begin(AudioCallback);
}

void loop() {
  // Take the button state
  button_is_pushed = digitalRead(kButtonPin);
  // Check if the state of the button changed from
  // released to pushed, ...
  if (button_is_pushed && !button_was_pushed) {
    // if yes then we excite the string...
    excite_string = 1;
    //...and play the next note.
    string.SetFreq(kNotes[next_note_index]);
    // Then move to the next note.
    next_note_index ++;
    // If the note we just played was the last 
    // in the sequence, we jump to the first one.
    if (next_note_index == kNotesCount) {
      next_note_index = 0;
    }
  }
  // Save the button state for the next iteration 
  button_was_pushed = button_is_pushed;
  // Wait 1ms until next interation, so we
  // are not quering the button too fast. It's
  // kind of debounce.
  delay(1);
}
