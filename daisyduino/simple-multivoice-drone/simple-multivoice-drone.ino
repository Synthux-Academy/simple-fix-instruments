#define S30 D15  // SWITCH // VOLUME ON-OFF
#define S31 A1  // KNOB // PITCH
#define S32 A2   // KNOB // OSCILLATOR FREQUENCY
#define S33 A3   // KNOB // TIMBRE

///////////////////////////////////////////////////////////////
///////////////////// LIBRARIES & HARDWARE ////////////////////
#include "vox.h"
#include "flt.h"

///////////////////////////////////////////////////////////////
///////////////////// MODULES /////////////////////////////////

const int kVoxCount = 10;
const float kVoxVolumeKof = 3.0 / kVoxCount;
Vox voxs[kVoxCount];

Filter flt;

bool gate = false;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK (PATCH) //////////////////
void AudioCallback(float **in, float **out, size_t size) {

  for (size_t i = 0; i < size; i++) {
    float output = 0;
    if (gate) {
      for (auto& vox: voxs) output += vox.Process(); 
      output = flt.Process(output) * kVoxVolumeKof;
    }
    out[0][i] = out[1][i] = output;
  }
}

///////////////////////////////////////////////////////////////
///////////////////// SETUP ///////////////////////////////////

void setup() {  
  // DAISY SETUP
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  auto sampleRate = DAISY.get_samplerate();

  // VOX SETUP
  float voxFreq = Vox::kOscLowestFreq;
  for (auto& vox: voxs) {
    vox.Init(sampleRate, voxFreq);
    voxFreq *= 1.5;
    if (voxFreq > 1500) voxFreq *= 0.5;
  }

  // FILTER SETUP
  flt.Init(sampleRate);

  // GATE SWITCH SETUP
  pinMode(S30, INPUT_PULLDOWN);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

///////////////////////////////////////////////////////////////
///////////////////// LOOP ////////////////////////////////////

void loop() {
  for (auto& vox: voxs) { vox.Read(S31, S32); }
  flt.Read(S33);
  gate = digitalRead(S30);
  delay(4);
}
