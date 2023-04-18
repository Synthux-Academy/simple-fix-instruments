/// Derived from 
/// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/string/string.ino
/// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/StereoDelay/StereoDelay.ino


#include "DaisyDuino.h"

#include <algorithm>

static DaisyHardware seed;
static StringOsc string;

// two DelayLine of 24000 floats.
DelayLine<float, 24000> del_left, del_right;

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
    // of the output buffer with String Oscilator output...
    float string_out = string.Process(excite_string);
    // ... and reset the flag so the string gets excited 
    // only on the first sample of the buffer
    excite_string = 0;

    // ################# DELAY ######################
    float dry_left, dry_right, wet_left, wet_right;

    // Read Dry from I/O
    dry_left = string_out;
    dry_right = string_out;

    // Read Wet from Delay Lines
    wet_left = del_left.Read();
    wet_right = del_right.Read();

    // Write to Delay with some feedback
    del_left.Write((wet_left * 0.5) + dry_left);
    del_right.Write((wet_right * 0.5) + dry_right);

    // Mix Dry and Wet and send to I/O
    out[0][i] = wet_left * 0.707 + dry_left * 0.707;
    out[1][i] = wet_right * 0.707 + dry_right * 0.707;
    // ###############################################
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

  // Init Delay Lines
  del_left.Init();
  del_right.Init();

  // Set Delay Times in Samples
  del_left.SetDelay(12000.0f);
  del_right.SetDelay(8000.0f);

  // Start Audio Callback
  DAISY.begin(AudioCallback);
}

void loop() {}