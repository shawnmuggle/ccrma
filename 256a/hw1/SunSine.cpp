#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <map>
#include <string.h>
using namespace std;

typedef double (*ugen) (double phase, double width);

double g_phase = 0;
double g_frequency = 220;
double g_width = 0.5;
ugen g_active_ugen;

double sine(double phase, double width)
{
  return sin(phase);
}

double pulse(double phase, double width)
{
  if (phase < 2 * M_PI * width)
    return 1.0;
  else
    return 0.0;
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

int callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	      double streamTime, RtAudioStreamStatus status, void *data )
{
  RtAudio *audio = (RtAudio *)data;
  unsigned int sampleRate = audio->getStreamSampleRate();

  for (unsigned int i = 0; i < nBufferFrames * 2;) {
    double increment =  (2.0 * M_PI) / sampleRate * g_frequency;
    g_phase += increment;
    if (g_phase > 2 * M_PI) {
      g_phase -= 2 * M_PI;
    }
    double samp = g_active_ugen(g_phase, g_width);

    // TODO: correctly loop over channels
    // TODO: find out if we're not supposed to be interleaved
    ((double *)outputBuffer)[i++] = samp;
    ((double *)outputBuffer)[i++] = samp;
  }

  return 0;
}

int usage()
{
  cout <<
    "SunSine [type] [frequency] [width]" << endl <<
    "    [type]: --sine | --saw | --pulse | --noise | --impulse" << endl <<
    "    [frequency]: (a number > 0, not required for noise" << endl <<
    "    [width]: pulse width ([0-1]), only required for saw and pulse" << endl;
  exit(1);
}

int main( int argc, char *argv[])
{
  map<string, ugen> ugens;
  ugens["--sine"] = &sine;
  ugens["--saw"] = &saw;
  ugens["--pulse"] = &pulse;
  ugens["--noise"] = &noise;
  ugens["--impulse"] = &impulse;

  //if (argc < 3 || argc > 4 ) usage();


  // TODO: find out what happens if this isn't stringable
  string type_arg = argv[1];
  g_active_ugen = ugens[type_arg];

  /*
  channels = (unsigned int) atoi( argv[1]);
  fs = (unsigned int) atoi( argv[2] );
  if ( argc > 3 )
    device = (unsigned int) atoi( argv[3] );
  if ( argc > 4 )
    offset = (unsigned int) atoi( argv[4] );
  */

  RtAudio audio;
  audio.showWarnings( true );

  RtAudio::StreamParameters params;

  // Choose an audio device
  unsigned int devices = audio.getDeviceCount();
  if ( devices < 1 ) {
    cerr << "No audio device found!" << endl;
    exit(1);
  }
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++ ) {
    info = audio.getDeviceInfo(i);
    if ( info.outputChannels > 1 ) {
      params.deviceId = i;
      params.nChannels = 2;
      break;
    }
  }
  params.firstChannel = 0;

  cout << "Using device ID " << params.deviceId << " which has " << 
    params.nChannels << " output channels." << endl;

  RtAudio::StreamOptions options;
  options.flags |= RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;

  // Choose a sample rate
  int sampleRate;
  if (info.sampleRates.size() < 1) {
    cerr << "No supported sample rates found!" << endl;
    exit(1);
  }
  for (int i = 0; i < info.sampleRates.size(); i++) {
    sampleRate = info.sampleRates[i];
    if (sampleRate == 44100 || sampleRate == 48000) {
      // Found a nice sample rate, stop looking
      break;
    }
  }
  cout << "Using sample rate: " << sampleRate << endl;

  unsigned int bufferFrames = 256;

  try {
    audio.openStream( &params,            // output params
		      NULL,               // input params
		      RTAUDIO_FLOAT64,    // audio format 
		      sampleRate,         // sample rate
		      &bufferFrames,      // num frames per buffer (mutable by rtaudio)
		      &callback,          // audio callback
		      &audio,             // user data pointer HACK HACK :D
		      &options);          // stream options
    audio.startStream();
  } catch ( RtError &e ) {
    e.printMessage();
    goto cleanup;
  }

  char input;
  cout << "Playing, press enter to quit (buffer frames = " << bufferFrames << ")." << endl;
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

  cout << "HELLO" << endl;
  return 0;
}
