/// Derived from https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/string/string.ino

#include "DaisyDuino.h"

#include <algorithm>

static DaisyHardware seed;
static StringOsc string;

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
uint8_t next_note_index = 0;

const int kButtonPin = 2;
bool button_was_pushed = false;

static void AudioCallback(float **in, float **out, size_t size) {
  uint8_t excite_string = 0;
  
  // Take the button state
  bool button_is_pushed = digitalRead(kButtonPin);
  // Check if the state of the button changed from
  // released to pushed, and if yes...
  if (button_is_pushed && !button_was_pushed) {
    //...then we excite the string...
    excite_string = 1;
    //...and play the next note.
    string.SetFreq(kNotes[next_note_index]);
   // Then move to the next note. If the note we 
   // just played was the last in the sequence,
   // we jump to the first one with "% kNotesCount"
    next_note_index = (next_note_index + 1) % kNotesCount;
  }
  // Save the button state for the next iteration 
  button_was_pushed = button_is_pushed;

  // Iterate through the number of samples in the buffer...
  for (size_t i = 0; i < size; i++) {
    // ... and fill both left and right channels 
    // of the output buffer with String Oscilator output
    out[0][i] = out[1][i] = string.Process(excite_string);
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

  // Setup button
  // In this case button is connected to
  // Digital 3v3 (Pin 38), so we need
  // to pull pin down
  pinMode(kButtonPin, INPUT_PULLDOWN);

  // Start Audio Callback
  DAISY.begin(AudioCallback);
}

void loop() {}