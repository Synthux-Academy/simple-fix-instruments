#define S30 D15  // SWITCH // VOLUME ON-OFF
#define S31 A1  // KNOB // PITCH
#define S32 A2   // KNOB // OSCILLATOR FREQUENCY
#define S33 A3   // KNOB // TIMBRE

///////////////////////////////////////////////////////////////
///////////////////// LIBRARIES & HARDWARE ////////////////////
#include "vox.h"

///////////////////////////////////////////////////////////////
///////////////////// MODULES /////////////////////////////////

const int kVoxCount = 15;
static Vox voxs[kVoxCount];

static Metro mtr;

int vox_idx = 0;
int arp_dir = 0;
float arp_rnd_chance = 0;
bool is_playing = false;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK (PATCH) //////////////////
void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (is_playing && mtr.Process()) {
      auto idx = vox_idx;
      if (arp_rnd_chance > 0.05 && arp_rnd_chance < 0.95) {
        auto rnd = static_cast<float>(random(0, 100)) / 100.f;  
        if (rnd <= arp_rnd_chance) {
          idx = random(0, kVoxCount - 1);
        }
      }

      voxs[idx].Trigger();
      vox_idx += arp_dir;
      if (arp_dir > 0) {
        vox_idx %= kVoxCount;
      }
      else if (vox_idx < 0) {
        vox_idx = kVoxCount - 1;
      }
    }

    float output = 0;
    for (auto& vox: voxs) output += vox.Process(); 
    out[0][i] = out[1][i] = output * 0.9;
  }
}

///////////////////////////////////////////////////////////////
///////////////////// SETUP ///////////////////////////////////

void setup() {  
  Serial.begin(9600);

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

  mtr.Init(8, sampleRate);

  // GATE SWITCH SETUP
  pinMode(S30, INPUT_PULLUP);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

///////////////////////////////////////////////////////////////
///////////////////// LOOP ////////////////////////////////////

void loop() {
  is_playing = digitalRead(S30);
  for (auto& vox: voxs) { vox.Read(S31, S32); }
  auto arp_ctr = analogRead(S33) / 1023.f;
  arp_dir = arp_ctr < .5f ? 1 : -1;
  arp_rnd_chance = arp_ctr < .5f ? 2.f * arp_ctr : 2.f * (1.f - arp_ctr);

  delay(4);
}
