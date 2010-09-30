#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <map>
#include <string.h>
using namespace std;

// TODO: clean up all globals into a state struct which gets passed back as user data
// TODO: create classes for ugens if we want to do per-ugen requirement checking on command line params
// FIXME: inaudibly wavering harmonic frequencies (at v. high harmonics)

typedef double (*ugen) (double phase, double width);

double g_phase = 0;
double g_frequency = 220;
double g_width = 0.5;
ugen g_active_ugen;

bool g_modulate_input;

bool g_fm_on = false;
double g_fm_phase = 0;
double g_fm_frequency = 300;
double g_fm_index = 20;
double g_fm_width = 0.5;
ugen g_active_fm_ugen;

// UGEN FUNCTIONS

double sine(double phase, double width)
{
  return sin(phase);
}

double pulse(double phase, double width)
{
  if (width == 0)
    width = 0.001;
  if (width == 1)
    width = 0.999;

  if (phase < 2 * M_PI * width)
    return 1.0;
  else
    return -1.0;
}

double saw(double phase, double width)
{
  if (phase < 2 * M_PI * width)
    return phase / (M_PI * width) - 1; // has a * 2 - 1 to normalize to double sample range
  else
    return 1 - 2 * (phase / (2 * M_PI) - width) / (1 - width); // get value over remaining range, then normalize to 1
}

double noise(double phase, double width)
{
  return (double)rand() / (double)RAND_MAX * 2 - 1;
}

double g_last_phase = 1.0;
double impulse(double phase, double width)
{
  double samp = 0.0;
  if (phase < g_last_phase)
    samp = 1.0;
  g_last_phase = phase;
  return samp;
}

// AUDIO CALLBACK

int callback( void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
	      double stream_time, RtAudioStreamStatus status, void *data )
{
  RtAudio *audio = (RtAudio *)data;
  unsigned int sample_rate = audio->getStreamSampleRate();

  for (unsigned int i = 0; i < n_buffer_frames * 2;) {
    
    double fm_offset = 0;
    if (g_fm_on) {
      double fm_increment =  (2.0 * M_PI) / sample_rate * g_fm_frequency;
      g_fm_phase += fm_increment;
      if (g_fm_phase > 2 * M_PI) {
	g_fm_phase -= 2 * M_PI;
      }
      fm_offset = g_active_fm_ugen(g_fm_phase, g_fm_width) * g_fm_index;
    }

    double increment =  (2.0 * M_PI) / sample_rate * (g_frequency + fm_offset);
    g_phase += increment;
    if (g_phase > 2 * M_PI) {
      g_phase -= 2 * M_PI;
    }
    double samp = g_active_ugen(g_phase, g_width);

    if (g_modulate_input) {
      samp = ((double *)input_buffer)[i / 2] * samp;
    }

    ((double *)output_buffer)[i++] = samp;
    ((double *)output_buffer)[i++] = samp;
  }

  return 0;
}

// USAGE MESSAGE

int usage()
{
  cout <<
    "SunSine ([type] [frequency] [width]) [modulation]" << endl <<
    "    [type]: --sine | --saw | --pulse | --noise | --impulse" << endl <<
    "    [frequency]: (a number > 0, not required for noise" << endl <<
    "    [width]: pulse width ([0-1]), only required for saw and pulse" << endl <<
    "    [modulation] (optional): " << endl << 
    "        --input (modulates the signal by the line/mic input)" << endl <<
    "        --fm (modulates the signal by another signal, requires another " << endl <<
    "            [type], [frequency], [width], and [index]" << endl <<
    "EXAMPLES:" << endl <<
    "    SunSine --noise 0 0 (ignores frequency and width)" << endl <<
    "    SunSine --sine 440 0 (ignores width)" << endl <<
    "    SunSine --pulse 220 0.5" << endl <<
    "    SunSine --saw 110 0.5 --input" << endl << 
    "    SunSine --saw 400 0.5 --fm --pulse 10 0.5 40" << endl <<
    "    SunSine --saw 400 0.5 --input --fm --pulse 10 0.5 100" << endl;
  exit(1);
}

int main( int argc, char *argv[])
{
  // COMMAND LINE ARG HANDLING
  map<string, ugen> ugens;
  ugens["--sine"] = &sine;
  ugens["--saw"] = &saw;
  ugens["--pulse"] = &pulse;
  ugens["--noise"] = &noise;
  ugens["--impulse"] = &impulse;

  if (argc < 4 || argc > 10 ) usage();

  string type_arg = argv[1];
  g_active_ugen = ugens[type_arg];
  if (g_active_ugen == NULL)
    usage();

  double freq_arg = atof(argv[2]);
  if (freq_arg <= 0)
    usage();
  g_frequency = freq_arg;

  double width_arg = atof(argv[3]);
  if (width_arg < 0 || width_arg > 1)
    usage();
  g_width = width_arg;

  if (argc > 4) { // modulation parameters present
    for (int i = 4; i < argc;) {
      if (string(argv[i]).compare("--input") == 0) {
	g_modulate_input = true;
	i++;
      } 
      else if (string(argv[i]).compare("--fm") == 0) {
	g_fm_on = true;

	string fm_type_arg = argv[++i];
	g_active_fm_ugen = ugens[fm_type_arg];
	if (g_active_fm_ugen == NULL)
	  usage();
	
	double fm_freq_arg = atof(argv[++i]);
	if (fm_freq_arg <= 0)
	  usage();
	g_fm_frequency = fm_freq_arg;
	
	double fm_width_arg = atof(argv[++i]);
	if (fm_width_arg < 0 || fm_width_arg > 1)
	  usage();
	g_fm_width = fm_width_arg;
	
	double fm_index_arg = atoi(argv[++i]);
	g_fm_index = fm_index_arg;

	i++;
      }
      else
	usage();
    }
  }

  // AUDIO SETUP
  RtAudio audio;
  audio.showWarnings( true );

  RtAudio::StreamParameters output_params;
  RtAudio::StreamParameters input_params;

  // Choose an audio device and a sample rate
  unsigned int sample_rate;
  unsigned int devices = audio.getDeviceCount();
  if ( devices < 1 ) {
    cerr << "No audio device found!" << endl;
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
	  break;
	}
      }
      cout << "Using sample rate: " << sample_rate << endl;

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
		      &callback,          // audio callback
		      &audio,             // user data pointer HACK HACK :D
		      &options);          // stream options
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

  return 0;
}
