#define S30 D15  // SWITCH // VOLUME ON-OFF
#define S31 A1  // KNOB // PITCH
#define S32 A2   // KNOB // OSCILLATOR FREQUENCY
#define S33 A3   // KNOB // TIMBRE

///////////////////////////////////////////////////////////////
///////////////////// LIBRARIES & HARDWARE ////////////////////
#include "vox.h"

///////////////////////////////////////////////////////////////
///////////////////// MODULES /////////////////////////////////

static const int kVoxCount = 15;
static const float kVoxVolumeKof = 1;
static Vox voxs[kVoxCount];
static Metro metro;

size_t vox_index = 0;
bool run = false;
bool pushed = false;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK (PATCH) //////////////////
void AudioCallback(float **in, float **out, size_t size) {

  for (size_t i = 0; i < size; i++) {
    float output = 0;
    if (run) {
      auto tick = metro.Process();
      if (tick) {
        auto p_index = vox_index;
        // if (random(0, 99) > 50) {
        //   p_index = random(0, kVoxCount);
        // }
        voxs[p_index].Trigger();
        vox_index ++;
        vox_index %= kVoxCount;
      }
      
      for (auto& vox: voxs) output += vox.Process();
  
    }
    out[0][i] = out[1][i] = output * 0.9;
  }
}

///////////////////////////////////////////////////////////////
///////////////////// SETUP ///////////////////////////////////

void setup() {  
  // DAISY SETUP
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  auto sampleRate = DAISY.get_samplerate();

  Serial.begin(9600);

  // VOX SETUP
  float voxFreq = Vox::kOscLowestFreq;
  for (auto& vox: voxs) {
    vox.Init(sampleRate, voxFreq);
    voxFreq *= 1.5;
    if (voxFreq > 1500) voxFreq *= 0.5;
  }

  metro.Init(4, sampleRate);

  // GATE SWITCH SETUP
  pinMode(S30, INPUT_PULLDOWN);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

///////////////////////////////////////////////////////////////
///////////////////// LOOP ////////////////////////////////////

void loop() {
  for (auto& vox: voxs) { vox.Read(S31, S32); }
  auto p = digitalRead(S30);
  if (p != pushed) {
    Serial.println("Pushed");
    if (p) { 
      Serial.println(p);
      run = !run; 
      }
    pushed = p;
  }
  
  delay(4);
}
