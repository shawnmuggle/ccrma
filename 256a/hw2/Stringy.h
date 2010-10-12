#include <vector>
using namespace std;

#define SAMPLE double

struct AudioData {
  double sample_rate;
};

// TODO: Add tick # to ugen.getsample calls, so they can kill a loop after the first time around

class UGen {
public:
  // TODO: THIS SHOULD BE PRIVATE but I can't get iterating over a private vector to work
  vector<UGen *> ugens;  
  
  virtual SAMPLE GetSample(double phase) = 0;
  void GetAudioFrom(UGen *ugen);
};

class Sine : public UGen {
public:
  virtual SAMPLE GetSample(double phase);
};

class DelayLine : public UGen {
  SAMPLE *delay_buffer;
  int max_delay_length, delay_length, write_index, read_index;
public:
  DelayLine(int max_delay_length, int delay_length);
  ~DelayLine();
  virtual SAMPLE GetSample(double phase);
};

class Gain :public UGen {
  double gain;
 public:
  Gain(double gain);
  virtual SAMPLE GetSample(double phase);
};

// TODO: MOVE FREQ & PHASE TO OSC/SYNTH UGENS
// TODO: give vector of ugens? It's more ggeennneerrrraaallll
class Voice {
  double phase;
  double frequency;
  UGen *ugen;
public:
  Voice(double frequency, UGen *unit_generator);
  ~Voice();
  SAMPLE GetSample(AudioData *audio_data);
};
