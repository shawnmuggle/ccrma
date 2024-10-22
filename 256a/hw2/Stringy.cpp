#include "Stringy.h"
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <map>
#include <string.h>
#include <assert.h>
#include <pthread.h>

double g_sample_rate = 0;
int g_num_voices = 30;
pthread_mutex_t voices_mutex;

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
  frequency(frequency)
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

// AUDIO CALLBACK (static!)
int Synth::AudioCallback(void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
			 double stream_time, RtAudioStreamStatus status, void *userData )
{
  deque<Voice *> *voices = (deque<Voice *> *) userData;
  for (unsigned int i = 0; i < n_buffer_frames * 2;) {
    SAMPLE output_sample = 0;
    
    pthread_mutex_lock(&voices_mutex);    
    deque<Voice *>::iterator itr=voices->begin();
    while(itr != voices->end()) {
      output_sample += (*itr)->GetSample();	
      ++itr;
    }
    pthread_mutex_unlock(&voices_mutex);
    
    ((SAMPLE *)output_buffer)[i++] = output_sample;
    ((SAMPLE *)output_buffer)[i++] = output_sample;
  }  
  return 0;
}

// static!!
double Synth::mtof(int pitch)
{
  double pitch_zero_freq = 8.17579892;
  return pitch_zero_freq * pow(2.0, pitch / 12.0);
}

// static!!
void Synth::MidiCallback(double deltatime, std::vector< unsigned char > *message, void *userData )
{
  deque<Voice *> *voices = (deque<Voice *> *) userData;
  unsigned int num_bytes = message->size();
  if ( num_bytes > 0 ) {
    cout << (int)message->at(0) << ", " << (int)message->at(1) << ", " << (int)message->at(2) << endl;
    pthread_mutex_lock(&voices_mutex);
    if ((int)message->at(0) == 144 && (int)message->at(2) > 0) {
      int freq = mtof((int)message->at(1));
      voices->push_back(new KarplusStrong(freq));
      if (voices->size() >= g_num_voices) {
	voices->pop_front();
      }
    } else if ((int)message->at(0) == 224) {
      double pitch_offset = ((int)message->at(2) - 64) / 64.0;
      deque<Voice *>::iterator itr=voices->begin();
      while(itr != voices->end()) {
	((KarplusStrong *)(*itr))->SetPitchOffset(pitch_offset);
	++itr;
      }
    } else if ((int)message->at(0) == 176) {
      double gain_offset = ((int)message->at(2) - 64) / 64.0;
      deque<Voice *>::iterator itr=voices->begin();
      while(itr != voices->end()) {
	((KarplusStrong *)(*itr))->SetGainOffset(gain_offset);
	++itr;
      }
    }
    pthread_mutex_unlock(&voices_mutex);
  }
}

Synth::Synth() :
  voices(new deque<Voice *>())
{}

Synth::~Synth ()
{
  delete voices;
  delete midi_in;
}

void Synth::SetUpMidi()
{
  // RtMidiIn constructor
  try {
    midi_in = new RtMidiIn();
  }
  catch (RtError &error) {
    // Handle the exception here
    error.printMessage();
  }

  unsigned int num_ports = midi_in->getPortCount();
  std::cout << "\nThere are " << num_ports << " MIDI input sources available.\n";
  std::string port_name;
  for ( unsigned int i=0; i<num_ports; i++ ) {
    try {
      port_name = midi_in->getPortName(i);
    }
    catch ( RtError &error ) {
      error.printMessage();
    }
    std::cout << "  Input Port #" << i+1 << ": " << port_name << '\n';
  }

  char input;
  cout << "Which device would you like to use for MIDI input?" << endl;
  cin >> input;
  cin.ignore();

  midi_in->setCallback(&Synth::MidiCallback, voices);
  midi_in->openPort(1);//(int)input - 49);
}

void Synth::SetUpAudio()
{
  audio.showWarnings( true );

  // Choose an audio device and a sample rate
  unsigned int sample_rate;
  unsigned int devices = audio.getDeviceCount();
  if ( devices < 1 ) {
    cerr << "No audio device found! (You probably need to start JACK)" << endl;
    exit(1);
  }
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++ ) {
    info = audio.getDeviceInfo(i);
    if ( info.isDefaultOutput ) {
      output_params.deviceId = i;
      output_params.nChannels = 2;

      if (info.sampleRates.size() < 1) {
	cerr << "No supported sample rates found!" << endl;
	exit(1);
      }
      for (int i = 0; i < info.sampleRates.size(); i++) {
	sample_rate = info.sampleRates[i];
	if (sample_rate == 44100 || sample_rate == 48000) {
	  // Found a nice sample rate, stop looking
	  g_sample_rate = sample_rate;
	  break;
	}
      }
      cout << "Using sample rate: " << g_sample_rate << endl;
    }
    if ( info.isDefaultInput ) {
      input_params.deviceId = i;
      input_params.nChannels = 1;
    }
  }

  cout << "Using output device ID " << output_params.deviceId << " which has " << 
    output_params.nChannels << " output channels." << endl;
  cout << "Using input device ID " << input_params.deviceId << " which has " << 
    input_params.nChannels << " input channels." << endl;

  RtAudio::StreamOptions options;
  options.flags |= RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;

  unsigned int buffer_frames = 256;
  try {
    audio.openStream( &output_params,
		      &input_params,
		      RTAUDIO_FLOAT64,
		      sample_rate,
		      &buffer_frames,
		      &Synth::AudioCallback,
		      voices,
		      &options);

    audio.startStream();
  } catch ( RtError &e ) {
    e.printMessage();
    goto cleanup;
  }

  char input;
  cout << "Playing, press enter to quit (buffer frames = " << buffer_frames << ")." << endl;
  cin.get( input );

  try {
    audio.stopStream();
  }
  catch ( RtError &e ) {
    e.printMessage();
  }

 cleanup:
  if ( audio.isStreamOpen() ) {
    audio.closeStream();
  }
}

int main( int argc, char *argv[])
{
  pthread_mutex_init(&voices_mutex, NULL);
  Synth *synth = new Synth();
  synth->SetUpMidi();
  synth->SetUpAudio();
  delete synth;
  return 0;
}
