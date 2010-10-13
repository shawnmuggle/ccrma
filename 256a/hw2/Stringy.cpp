#include "Stringy.h"
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <map>
#include <string.h>
#include <assert.h>

// FIXME: inaudibly wavering harmonic frequencies (at v. high harmonics)

double g_sample_rate = 0;
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
  if (this == NULL) {
    cout << "this == NULL??!!" << endl;
  }
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
  write_index(0)
{
  //cout << "Delay length: " << delay_length << ", MAX delay length: " << max_delay_length << endl;

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
  //cout << "WHATdelay" << endl;  
  delete delay_buffer;
}

SAMPLE DelayLine::ComputeOutputSample(SAMPLE input_sample)
{
  delay_buffer[write_index++] = input_sample;
  if (write_index >= max_delay_length) {
    write_index -= max_delay_length;
  }
  
  SAMPLE output_sample = delay_buffer[read_index++];
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

Voice::Voice()
{}

Voice::Voice(UGen *ugen) : 
  ugen(ugen),
  tick_count(0)
{}

Voice::~Voice()
{
  //cout << "WHATvoice" << endl;
  delete ugen;
}

SAMPLE Voice::GetSample()
{
  return ugen->GetSample(tick_count++);
}

KarplusStrong::KarplusStrong(double frequency)
{
  int wavelength_in_samples = (int) (g_sample_rate / frequency);

  UGen *noise = new WhiteNoise();
  RectangularEnvelope *env = new RectangularEnvelope(wavelength_in_samples, 1.0);
  UGen *moving_average = new MovingAverage(1000, 2);
  UGen *delay = new DelayLine(44100 * 5, wavelength_in_samples);
  UGen *gain = new Gain(0.99);
  
  env->GetAudioFrom(noise);
  delay->GetAudioFrom(env);
  moving_average->GetAudioFrom(delay);
  gain->GetAudioFrom(moving_average);
  delay->GetAudioFrom(gain);

  this->ugen = delay;
}

// AUDIO CALLBACK (static!)
int Synth::AudioCallback(void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
		    double stream_time, RtAudioStreamStatus status, void *userData )
{
  //cout << "BOO" << endl;
  vector<Voice *> *voices = (vector<Voice *> *) userData;
  //cout << 2 << "--------------------------------------" << endl;
  for (unsigned int i = 0; i < n_buffer_frames * 2;) {
    //cout << 3 << "." << i << endl;
    SAMPLE output_sample = 0;
    
    vector<Voice *>::iterator itr;
    //pthread_mutex_lock(&voices_mutex);
    for(itr=voices->begin(); itr!=voices->end(); itr++) {
      //cout << 4 << endl;
      if (*itr == NULL) {
	cout << "VOICE IS NULL !?!?!" << endl;
      }
      output_sample += (*itr)->GetSample();
    }
    //pthread_mutex_unlock(&voices_mutex);
    
    ((SAMPLE *)output_buffer)[i++] = output_sample;
    ((SAMPLE *)output_buffer)[i++] = output_sample;
  }  
  return 0;
}

// static!!
void Synth::MidiCallback(double deltatime, std::vector< unsigned char > *message, void *userData )
{
  //cout << "HELLO" << endl;

  vector<Voice *> *voices = (vector<Voice *> *) userData;

  //cout << voices->size() << endl;

  unsigned int num_bytes = message->size();
  //  for (unsigned int i = 0; i < num_bytes; i++) {
    //std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
  //}
  if ( num_bytes > 0 ) {
    //std::cout << "stamp = " << deltatime << std::endl;
    
    //pthread_mutex_lock(&voices_mutex);
    if ((int)message->at(0) == 144) {
      voices->push_back(new KarplusStrong(440));
    }
    //pthread_mutex_unlock(&voices_mutex);
  }
}

Synth::Synth() :
  voices(new vector<Voice *>())
{}

Synth::~Synth ()
{
  //cout << "WHATsynth" << endl;
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
  //std::cout << "\nThere are " << num_ports << " MIDI input sources available.\n";
  std::string port_name;
  for ( unsigned int i=0; i<num_ports; i++ ) {
    try {
      port_name = midi_in->getPortName(i);
    }
    catch ( RtError &error ) {
      error.printMessage();
    }
    //std::cout << "  Input Port #" << i+1 << ": " << port_name << '\n';
  }

  midi_in->setCallback(&Synth::MidiCallback, voices);
  midi_in->openPort();
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
    audio.openStream( &output_params,     // output params
		      &input_params,      // input params
		      RTAUDIO_FLOAT64,    // audio format 
		      sample_rate,        // sample rate
		      &buffer_frames,     // num frames per buffer (mutable by rtaudio)
		      &Synth::AudioCallback,          // audio callback
		      voices,        // user data pointer
		      &options);          // stream options

    audio.startStream();


    voices->push_back(new KarplusStrong(100));
    voices->push_back(new KarplusStrong(150));
    voices->push_back(new KarplusStrong(200));


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
  Synth *synth = new Synth();
  synth->SetUpMidi();
  synth->SetUpAudio();
  delete synth;
  return 0;
}
