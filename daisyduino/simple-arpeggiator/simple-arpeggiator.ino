
#include "vox.h"
#include "arp.h"
#include "term.h"
#include "scale.h"

using namespace synthux;

////////////////////////////////////////////////////////////
////////////////////////// CONTROLS ////////////////////////

#define S30 D15 // SWITCH : START / STOP
#define S31 A1  // KNOB : SPEED
#define S32 A2  // KNOB : CHORD
#define S33 A3  // KNOB : DIRECTION / RANDOM

static const int kAnalogResolution  = 7; //7bits => 0..127
static const float kKnobMax = powf(2.f, kAnalogResolution) - 1.f;

////////////////////////////////////////////////////////////
///////////////////// MODULES //////////////////////////////

static const uint8_t kNotesCount = 12;
static const uint8_t kScaleSize = 36;
static const uint8_t kPPQN = 24;

static Scale<kScaleSize> scale;
static Terminal<kNotesCount, kScaleSize> term;
static Arp<kNotesCount, kPPQN> arp;
static Metro metro;
static Vox vox;

////////////////////////////////////////////////////////////
///////////////////// CALLBACKS ////////////////////////////

void OnTerminalNoteOn(uint8_t num, uint8_t vel) { arp.NoteOn(num, vel); }
void OnTerminalNoteOff(uint8_t num) { arp.NoteOff(num); }
void OnArpNoteOn(uint8_t num, uint8_t vel) { vox.NoteOn(scale.freqAt(num)); }
void OnArpNoteOff(uint8_t num) { vox.NoteOff(); }

bool is_playing = false;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK //////////////////////////
void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (is_playing && metro.Process()) arp.Trigger();
    out[0][i] = out[1][i] = vox.Process();
  }
}

///////////////////////////////////////////////////////////////
////////////////////////// SETUP //////////////////////////////

void setup() {  
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  auto sample_rate = DAISY.get_samplerate();

  vox.Init(sample_rate);

  metro.Init(48, sample_rate); //48Hz = 24ppqn @ 120bpm 

  term.SetOnNoteOn(OnTerminalNoteOn);
  term.SetOnNoteOff(OnTerminalNoteOff);

  arp.SetOnNoteOn(OnArpNoteOn);
  arp.SetOnNoteOff(OnArpNoteOff);
  
  pinMode(S30, INPUT_PULLDOWN);

  analogReadResolution(kAnalogResolution);

  DAISY.begin(AudioCallback);
}

///////////////////////////////////////////////////////////////
////////////////////////// LOOP ///////////////////////////////

void loop() {
  is_playing = digitalRead(S30);

  auto speed = analogRead(S31) / kKnobMax;
  auto freq = 16.f + 80.f * speed; // for 24 PPQN
  metro.SetFreq(freq); 

  auto offset = analogRead(S32) / kKnobMax; 
  term.SetOffset(offset);
  
  auto arp_ctr = analogRead(S33) / kKnobMax;
  auto arp_dir = arp_ctr < .5f ? ArpDirection::fwd : ArpDirection::rev;
  auto arp_rnd = arp_ctr < .5f ? 2.f * arp_ctr : 2.f * (1.f - arp_ctr);
  arp.SetDirection(arp_dir);
  arp.SetRandChance(arp_rnd);
}
