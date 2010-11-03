#include "ugens.h"
#include "math.h"
#include "assert.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

double g_sample_rate = 44100;

UGen::UGen () :
  last_tick_seen(-1),
  last_sample_generated(0)
{}

void UGen::GetAudioFrom(UGen *ugen)
{
  ugens.push_back(ugen);
}

SAMPLE UGen::GetSample(int tick_count)
{
  if (tick_count > last_tick_seen) {
    last_tick_seen = tick_count;
    SAMPLE input_sample = ComputeInputSample(tick_count);
    last_sample_generated = ComputeOutputSample(input_sample);
  }
  return last_sample_generated;
}

SAMPLE UGen::ComputeInputSample(int tick_count)
{
  SAMPLE input_sample = 0;
  vector<UGen *>::iterator itr;
  for(itr=ugens.begin(); itr!=ugens.end(); itr++) {
    input_sample += (*itr)->GetSample(tick_count);
  }
  return input_sample;
}

Sine::Sine(double frequency) :
  frequency(frequency),
  phase(0)
{}

SAMPLE Sine::ComputeOutputSample(SAMPLE input_sample)
{
  double increment =  (2.0 * M_PI) / g_sample_rate * (frequency);
  phase += increment;
  if (phase > 2 * M_PI) {
    phase -= 2 * M_PI;
  }
  return sin(phase);
}

void Sine::SetFrequency(double freq)
{
  frequency = freq;
}

SAMPLE WhiteNoise::ComputeOutputSample(SAMPLE input_sample)
{
  return (double)rand() / (double)RAND_MAX * 2 - 1;
}

DelayLine::DelayLine(int max_delay_length, int delay_length) :
  max_delay_length(max_delay_length),
  delay_length(delay_length),
  delay_length_offset(0),
  write_index(0)
{
  assert(delay_length <= max_delay_length);

  delay_buffer = new SAMPLE[max_delay_length];
  memset(delay_buffer, 0, sizeof(SAMPLE) * max_delay_length);
  read_index = write_index - delay_length;
  if (read_index < 0) {
    read_index += max_delay_length;
  }
}

DelayLine::~DelayLine()
{
  delete delay_buffer;
}

SAMPLE DelayLine::ComputeOutputSample(SAMPLE input_sample)
{
  delay_buffer[write_index++] = input_sample;
  if (write_index >= max_delay_length) {
    write_index -= max_delay_length;
  }
  
  int read_position = (read_index + delay_length_offset) % max_delay_length;
  if (read_position < 0) {
    read_position += max_delay_length;
  }
  SAMPLE output_sample = delay_buffer[read_position];
  read_index += 1;
  if (read_index >= max_delay_length) {
    read_index -= max_delay_length;
  }
  
  return output_sample;
}

Gain::Gain(double gain) :
  gain(gain)
{}

SAMPLE Gain::ComputeOutputSample(SAMPLE input_sample)
{
  return input_sample * gain;
}

void Gain::SetGain(double gain)
{
  gain = gain;
}

MovingAverage::MovingAverage(int max_average_length, int average_length) :
  max_average_length(max_average_length),
  average_length(average_length),
  write_index(0),
  sum(0)
{
  assert(average_length <= max_average_length);

  sample_values = new SAMPLE[max_average_length];
  memset(sample_values, 0, sizeof(SAMPLE) * max_average_length);
}

SAMPLE MovingAverage::ComputeOutputSample(SAMPLE input_sample)
{
  SAMPLE output_sample = sum / average_length;
  sum -= sample_values[write_index];
  sample_values[write_index] = input_sample;
  sum += sample_values[write_index];

  write_index++;
  if (write_index >= average_length) {
    write_index -= average_length;
  }

  return output_sample;
}

MovingAverage::~MovingAverage ()
{
  delete sample_values;
}

RectangularEnvelope::RectangularEnvelope(int length, double amplitude) :
  length(length),
  amplitude(amplitude),
  samples_seen(0)
{}

SAMPLE RectangularEnvelope::ComputeOutputSample(SAMPLE input_sample)
{
  if (samples_seen++ < length) {
    return input_sample * amplitude;
  } else {
    return 0.0;
  }
}

AREnvelope::AREnvelope(int attack_length, int release_length, double amplitude) :
  attack_length(attack_length),
  release_length(release_length),
  amplitude(amplitude),
  samples_seen(0)
{}

SAMPLE AREnvelope::ComputeOutputSample(SAMPLE input_sample)
{
  double scale = 0.0;
  if (samples_seen < attack_length) {
    scale = amplitude * samples_seen / (float)attack_length;
  } else if (samples_seen < attack_length + release_length) {
    scale = amplitude * (1 - ((samples_seen - attack_length) / (float)release_length));
  }
  ++samples_seen;
  return input_sample * scale;
}

void AREnvelope::Reset()
{
  samples_seen = 0;
}

// Forgot that I needed this for a couple hours, which caused me great anguish!
//   (Also caused me to learn how to use gdb and valgrind, so that's good)
Voice::Voice() :
  tick_count(0)
{}

Voice::Voice(UGen *ugen) : 
  ugen(ugen),
  tick_count(0)
{}

Voice::~Voice()
{
  delete ugen;
}

SAMPLE Voice::GetSample()
{
  return ugen->GetSample(tick_count++);
}

KarplusStrong::KarplusStrong(double frequency) :
  pitch_offset_scale(1000)
{
  int wavelength_in_samples = (int) (g_sample_rate / frequency);
  int max_delay_length = 44100 * 5;

  WhiteNoise *noise = new WhiteNoise();
  RectangularEnvelope *env = new RectangularEnvelope(wavelength_in_samples, 1.0);
  MovingAverage *average = new MovingAverage(1000, 2);
  delay = new DelayLine(max_delay_length, wavelength_in_samples);
  gain = new Gain(0.99999);
  
  env->GetAudioFrom(noise);
  delay->GetAudioFrom(env);
  average->GetAudioFrom(delay);
  gain->GetAudioFrom(average);
  delay->GetAudioFrom(gain);
  
  ugen = delay;
}

// offset is from [-1, 1]
void KarplusStrong::SetPitchOffset(double offset)
{
  delay->delay_length_offset = offset * pitch_offset_scale;
}

// offset is from [-1, 1]
void KarplusStrong::SetGainOffset(double offset)
{
  gain->gain = .9 + offset * .2;
}

double mtof(int pitch)
{
  static double pitch_zero_freq = 8.17579892;
  return pitch_zero_freq * pow(2.0, pitch / 12.0);
}
