//
//  WOSynth.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOSynth.h"

#import "mo_audio.h"

#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2

// the audio callback
void audioCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    for ( int i = 0; i < frameSize; i++ )
    {
        Float32 sample = (2 * rand() / (float)RAND_MAX) - 1;
        
        buffer[i * 2] = buffer[i * 2 + 1] = sample;
    } 
}

@implementation WOSynth

- (id) init
{
    self = [super init];
    if (self) {
        // init the audio layer
        bool result = MoAudio::init( SRATE, FRAMESIZE, NUMCHANNELS );
        if( !result )
        {
            // something went wrong
            NSLog( @"cannot initialize real-time audio!" );
            // bail out
            return self;
        }
        
        // start the audio layer, registering a callback method
        result = MoAudio::start( audioCallback, NULL);
        if( !result )
        {
            // something went wrong
            NSLog( @"cannot start real-time audio!" );
            // bail out
            return self;
        }
    }
    return self;
}

@end
