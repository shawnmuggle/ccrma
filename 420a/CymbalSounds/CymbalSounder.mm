//
//  CymbalSounder.m
//  CymbalSounds
//
//  Created by Michael Rotondo on 2/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "CymbalSounder.h"

#import "RtAudio.h"
#import "Mesh2D.h"

stk::Mesh2D* mesh;

// Two-channel sawtooth wave generator.
int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *userData )
{
    unsigned int i, j;
    double *buffer = (double *) outputBuffer;
    
    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
    
    // Write interleaved audio data.
    for ( i=0; i<nBufferFrames; i++ ) {
        for ( j=0; j<2; j++ ) {
            *buffer++ = mesh->tick();
        }
    }
    
    return 0;
}

void* startThread(void* aDac)
{
    RtAudio* dac = (RtAudio*)aDac;
    
    mesh = new stk::Mesh2D(12, 12);
    mesh->setInputPosition(0.5, 0.5);
    mesh->setDecay(1.0);
    mesh->noteOn(10.0, 1.0);
    
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac->getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256; // 256 sample frames
    double data[2];
    
    try {
        dac->openStream( &parameters, NULL, RTAUDIO_FLOAT64,
                       sampleRate, &bufferFrames, &saw, (void *)&data );
        dac->startStream();
    }
    catch ( RtError& e ) {
        e.printMessage();
        exit( 0 );
    }
    
    return NULL;
}

@implementation CymbalSounder

- (id) init
{
    self = [super init];
    if (self) {
        RtAudio* dac = new RtAudio();
        
        NSLog(@"Current api: %d", dac->getCurrentApi());
        
        if ( dac->getDeviceCount() < 1 ) {
            std::cout << "\nNo audio devices found!\n";
            exit( 0 );
        }

        pthread_create(&rtaudio_thread, NULL, startThread, (void*)dac);
//        
//        try {
//            // Stop the stream
//            dac.stopStream();
//        }
//        catch (RtError& e) {
//            e.printMessage();
//        }
//        
//        if ( dac.isStreamOpen() ) dac.closeStream();
//        
//        return 0;
    }
    return self;
}

@end
