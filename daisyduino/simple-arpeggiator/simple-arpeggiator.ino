#define S30 D15  // SWITCH // VOLUME ON-OFF
#define S31 A1  // KNOB // PITCH
#define S32 A2   // KNOB // OSCILLATOR FREQUENCY
#define S33 A3   // KNOB // TIMBRE

///////////////////////////////////////////////////////////////
///////////////////// LIBRARIES & HARDWARE ////////////////////
#include "vox.h"
#include "arp.h"
#include "term.h"
#include "scale.h"

///////////////////////////////////////////////////////////////
///////////////////// MODULES /////////////////////////////////

using namespace synthux;

static Vox vox;
static Metro metro;
static Scale scale;
static Terminal term;
static Arp arp;

void OnTerminalNoteOn(int32_t num) { arp.NoteOn(num); }
void OnTerminalNoteOff(int32_t num) { arp.NoteOff(num); }
void OnArpNoteOn(int32_t num) { vox.NoteOn(scale.freqAt(num)); }
void OnArpNoteOff(int32_t num) { vox.NoteOff(); }

bool is_playing = false;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK (PATCH) //////////////////
void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (is_playing && metro.Process()) arp.Trigger();
    out[0][i] = out[1][i] = vox.Process();
  }
}

///////////////////////////////////////////////////////////////
///////////////////// SETUP ///////////////////////////////////

void setup() {  
  // DAISY SETUP
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  auto sample_rate = DAISY.get_samplerate();

//  Serial.begin(9600);

  // VOX SETUP
  vox.Init(sample_rate);

  metro.Init(48, sample_rate); //120bpm 24ppqn

  term.SetOnNoteOn(OnTerminalNoteOn);
  term.SetOnNoteOff(OnTerminalNoteOff);

  arp.SetOnNoteOn(OnArpNoteOn);
  arp.SetOnNoteOff(OnArpNoteOff);
  
  // GATE SWITCH SETUP
  pinMode(S30, INPUT_PULLDOWN);

  analogReadResolution(7);

  // BEGIN CALLBACK
  DAISY.begin(AudioCallback);
}

///////////////////////////////////////////////////////////////
///////////////////// LOOP ////////////////////////////////////

void loop() {
  is_playing = digitalRead(S30);

  auto speed = analogRead(S31) / 127.f;
  auto freq = 16.f + 80.f * speed;
  metro.SetFreq(freq); 

  auto notes = analogRead(S32) / 127.f; 
  term.Read(notes);
  
  auto arp_ctr = analogRead(S33) / 127.f;
  auto arp_dir = arp_ctr < .5f ? Arp::Direction::up : Arp::Direction::down;
  auto arp_rnd = arp_ctr < .5f ? 2.f * arp_ctr : 2.f * (1.f - arp_ctr);
  arp.SetDirection(arp_dir);
  arp.SetRandomize(arp_rnd);

  delay(4);
}

