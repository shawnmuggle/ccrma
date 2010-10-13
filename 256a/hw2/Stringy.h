#include "RtAudio.h"
#include "rtmidi/RtMidi.h"
#include <deque>
using namespace std;

#define SAMPLE double

/*
 * The top level class here is Synth, which has a deque of Voices, each of which owns
 * a UGen which is actually one node in a graph of UGens. Synth owns the MIDI and audio
 * callbacks. 
 *
 * In the MIDI callback, the deque of Voices is added to (and subtracted from
 * when a new voice is added and the polyphony capacity has already been reached). In the
 * audio callback, for each sample being supplied to the audio hardware, each Voice is
 * asked for its next sample. 
 *
 * The Voices, in turn, ask their UGen for its next sample. They provide their UGens with 
 * tick counts, which are distinct to each Voice, so that potential loops in the graph 
 * of UGens can be handled. Each UGen, if it hasn't yet computed a sample for this tick,
 * passes on the request for a new sample to its suppliers, computes a new sample given 
 * the combined inputs and increments its state as necessary. Otherwise, it just returns
 * the sample it has already computed for this tick.
 * 
 * UGens chain together with GetAudioFrom calls, like so:
 * UGen *sine = new Sine(440);
 * UGen *sine2 = new Sine(220);
 * UGen *gain = new Gain(0.5);
 * gain->GetsAudioFrom(sine);
 * gain->GetsAudioFrom(sine2);
 */

class UGen {
  int last_tick_seen;
  vector<UGen *> ugens;  
 protected:
  SAMPLE last_sample_generated;
 public:  
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
  int delay_length_offset;
  DelayLine(int max_delay_length, int delay_length);
  ~DelayLine();
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
};

class Gain : public UGen {
 public:
  double gain;
  Gain(double gain);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
  void SetGain(double gain);
};

class MovingAverage : public UGen {
  SAMPLE *sample_values;
  int average_length;
  int max_average_length;
  int write_index;
  SAMPLE sum;
 public:
  ~MovingAverage();
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
  int tick_count;
  UGen *ugen;
 public:
  Voice();
  Voice(UGen *unit_generator);
  ~Voice();
  SAMPLE GetSample();
};

class KarplusStrong : public Voice {
  double pitch_offset_scale;
  DelayLine *delay;
  Gain *gain;
 public:
  KarplusStrong(double frequency);
  void SetPitchOffset(double offset); 
  void SetGainOffset(double gain); 
};

class Synth {
  RtAudio audio;
  RtMidiIn *midi_in;
  RtAudio::StreamParameters output_params;
  RtAudio::StreamParameters input_params;

  deque<Voice *> *voices;
  static int AudioCallback(void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
			   double stream_time, RtAudioStreamStatus status, void *data);
  static void MidiCallback(double deltatime, std::vector< unsigned char > *message, void *userData);
  static double mtof(int pitch);
 public:
  Synth();
  ~Synth();
  void SetUpAudio();
  void SetUpMidi();
  void AddVoice(Voice *voice);
};
