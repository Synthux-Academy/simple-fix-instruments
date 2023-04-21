
#include "DaisyDuino.h"
#include "simplestring.h"
#include "simplebutton.h"
#include "simplemelody.h"

static DaisyHardware seed;

static SimpleString string;
static SimpleMelody melody;
static SimpleButton button;

static void AudioCallback(float **in, float **out, size_t size) {
  string.play(size, out);
}

void setup() {
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  string.setup();
  button.setup();

  melody.add_note(C4);
  melody.add_note(F4);
  melody.add_note(A4);

  DAISY.begin(AudioCallback);
}

void loop() {
  if (button.is_pressed()) string.pluck(melody.note());
}
