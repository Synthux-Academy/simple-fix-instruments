/// Derived from 
/// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/string/string.ino
/// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/StereoDelay/StereoDelay.ino

#include "DaisyDuino.h"

static DaisyHardware seed;
static StringOsc string;

//Define delay lines
const int kDelayBufferLength = 24000; //500ms @ 48KHz sample rate. kDelayLeftTime and kDelayRightTime can not be longer than this.
DelayLine<float, kDelayBufferLength> delay_left;
DelayLine<float, kDelayBufferLength> delay_right;

// Define delay parameters
const float kDelayLeftTime = 12000; //250ms @ 48KHz sample rate. Should not be longer than kDelayBufferLength
const float kDelayRightTime = 8000; //~166ms @ 48KHz sample rate Should not be longer than kDelayBufferLength
const float kDelayFeedback = 0.8; // 0...1
const float kDelayWetMix = 0.33; // 0...1

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
// it to 1 on button push and reset to 0
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

    // Add Delay ##################################
    // Read dry from string
    float dry_left = string_out;
    float dry_right = string_out;

    // Read Wet from delay lines
    float wet_left = delay_left.Read();
    float wet_right = delay_right.Read();

    // Write to Delay with feedback
    delay_left.Write((wet_left * kDelayFeedback) + dry_left);
    delay_right.Write((wet_right * kDelayFeedback) + dry_right);

    // Mix Dry and Wet and send to output
    out[0][i] = wet_left * kDelayWetMix + dry_left * (1 - kDelayWetMix);
    out[1][i] = wet_right * kDelayWetMix + dry_right * (1 - kDelayWetMix);
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

   // Init Delay Lines
  delay_left.Init();
  delay_right.Init();

  // Set Delay Times in Samples
  delay_left.SetDelay(kDelayLeftTime);
  delay_right.SetDelay(kDelayRightTime);

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
