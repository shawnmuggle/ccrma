#include "RtAudio.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define SAMPLE double
#define MY_PIE 3.14159265358979
#define MY_SRATE 44100

SAMPLE g_freq = 880;
SAMPLE g_t = 0;

// callback
int callme( char * buffer, int buffer_size, void * user_data )
{
    fprintf( stderr, "." );
    SAMPLE * buffy = (SAMPLE *)buffer;
    
    for( int i = 0; i < buffer_size; i++ )
    {
        buffy[i] = sin( 2 * MY_PIE * g_freq * g_t / MY_SRATE );
        g_t += 1.0;
    }
    
    return 0;
}

// entry point
int main( int argc, char ** argv )
{
    // RtAudio pointer
    RtAudio * audio = NULL;
    // buffer size
    int buffer_size = 512;
    
    // create the RtAudio
    try {
        audio = new RtAudio(
            0, // device number of output
            1, // number of output channels
            0, // device number for input
            0, // number of input channels
            RTAUDIO_FLOAT64, // format
            MY_SRATE, // sample rate
            &buffer_size, // buffer size
            8 // number of buffers
        );
    } catch( RtError & err ) {
        err.printMessage();
        exit(1);
    }
    
    // set the callback
    try {
        audio->setStreamCallback( &callme, NULL );
        audio->startStream();
    } catch( RtError & err ) {
        // do stuff
        err.printMessage();
        goto cleanup;
    }
    
    // something needs to happen here
    while( true )
    {
        usleep( 10000 );
    }
    
    // if we get here, then stop!
    try {
        audio->stopStream();
    } catch( RtError & err ) {
        // do stuff
        err.printMessage();
    }

cleanup:
    audio->closeStream();
    delete audio;
    
    return 0;
}
