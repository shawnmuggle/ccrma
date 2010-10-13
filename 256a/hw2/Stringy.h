#include "RtAudio.h"
#include "rtmidi/RtMidi.h"
#include <vector>
#include <pthread.h>
using namespace std;

#define SAMPLE double

// TODO: Figure out a way to determine when a voice is "done" (probably when it can recursively call "IsDone" and get a full termination on all sub-paths of UGens [gonna have to figure out loops again])

class UGen {
  int last_tick_seen;
  SAMPLE last_sample_generated;
 public:
  // TODO: THIS SHOULD BE PRIVATE but I can't get iterating over a private vector to work
  vector<UGen *> ugens;  
  
  UGen();
  void GetAudioFrom(UGen *ugen);
  SAMPLE GetSample(int tick_count);
  SAMPLE ComputeInputSample(int tick_count);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample) = 0;
};

class Sine : public UGen {
  double phase;
  double frequency;
 public:
  Sine(double frequency);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
};

class WhiteNoise : public UGen {
 public:
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
};

class DelayLine : public UGen {
  SAMPLE *delay_buffer;
  int max_delay_length, delay_length, write_index, read_index;
 public:
  DelayLine(int max_delay_length, int delay_length);
  ~DelayLine();
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
  SAMPLE AverageValue(); // pretty random, but I need to find out what's been coming out
};

class Gain : public UGen {
  double gain;
 public:
  Gain(double gain);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
};

class MovingAverage : public UGen {
  SAMPLE *sample_values;
  int average_length;
  int max_average_length;
  int write_index;
  SAMPLE sum;
 public:
  MovingAverage(int max_average_length, int average_length);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
};

class RectangularEnvelope : public UGen {
  int length;
  double amplitude;
  int samples_seen;
 public:
  RectangularEnvelope(int length, double amplitude);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
};

class Voice {
 protected:
  //SHOULD BE PRIVATE but is used from a public method for now
  int tick_count;
  UGen *ugen;
 public:
  Voice();
  Voice(UGen *unit_generator);
  ~Voice();
  SAMPLE GetSample();
  virtual bool IsDone();
};

class KarplusStrong : public Voice {
 public:
  KarplusStrong(double frequency);
  virtual bool IsDone();
};

class Synth {
  RtAudio audio;
  RtMidiIn *midi_in;
  RtAudio::StreamParameters output_params;
  RtAudio::StreamParameters input_params;

  vector<Voice *> *voices;
  static int AudioCallback(void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
			   double stream_time, RtAudioStreamStatus status, void *data);
  static void MidiCallback(double deltatime, std::vector< unsigned char > *message, void *userData);
 public:
  Synth();
  ~Synth();
  void SetUpAudio();
  void SetUpMidi();
  void AddVoice(Voice *voice);
};
