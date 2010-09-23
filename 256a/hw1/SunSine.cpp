#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <math.h>
using namespace std;

int callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	      double streamTime, RtAudioStreamStatus status, void *data )
{
  RtAudio *audio = (RtAudio *)data;
  //cout << audio->getStreamSampleRate() << endl;

  for (unsigned int i = 0; i < nBufferFrames * 2;) {
    double samp = ((double) rand() / (double) RAND_MAX) * 2 - 1;
    //cout << samp << endl;
    ((double *)outputBuffer)[i++] = samp;
    ((double *)outputBuffer)[i++] = samp;
  }

  return 0;
}

int main( )
{
  RtAudio audio;

  unsigned int devices = audio.getDeviceCount();
  if ( devices < 1 ) {
    cerr << "No audio device found!" << endl;
    exit(1);
  }

  audio.showWarnings( true );

  unsigned int bufferFrames = 256;
  
  RtAudio::StreamParameters params;
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++ ) {
    info = audio.getDeviceInfo(i);
    if ( info.outputChannels > 0 ) {
      params.deviceId = i;
      params.nChannels = 2;//info.outputChannels;
      break;
    }
  }
  params.firstChannel = 0;

  cout << "Using device ID " << params.deviceId << " which has " << 
    params.nChannels << " output channels." << endl;

  RtAudio::StreamOptions options;
  options.flags |= RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;

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
