#include <vector>
using namespace std;

#define SAMPLE double

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
  void SetFrequency(double freq);
};

class Pulse : public UGen {
  double phase;
  double width;
  double frequency;
 public:
  Pulse(double frequency, double width);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
  void SetFrequency(double freq);
  void SetWidth(double width);
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

class AREnvelope : public UGen {
  int attack_length;
  int release_length;
  double amplitude;
  int samples_seen;
 public:
  AREnvelope(int attack_length, int release_length, double amplitude);
  virtual SAMPLE ComputeOutputSample(SAMPLE input_sample);
  void Reset();
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

double mtof(int pitch);
