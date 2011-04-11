//
//  WOSynth.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOSynth.h"

#import "mo_audio.h"
#import "WOTree.h"

#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2

// the audio callback
void audioCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    WOState* state = (WOState*)userData;
    
    stk::StkFrames frames(frameSize, 1);

    for (WOTree* tree in state.currentPlanet.trees) {
        [tree tickAudio:&frames];
    }
        
    for ( int i = 0; i < frameSize; i++ )
    {
        buffer[i * 2] = buffer[i * 2 + 1] = frames[i];
    } 
}

@implementation WOSynth

- (void) awakeFromNib
{
    // init the audio layer
    bool result = MoAudio::init( SRATE, FRAMESIZE, NUMCHANNELS );
    if( !result )
    {
        // something went wrong
        NSLog( @"cannot initialize real-time audio!" );
        // bail out
        return;
    }
    
    // start the audio layer, registering a callback method
    result = MoAudio::start( audioCallback, state);
    if( !result )
    {
        // something went wrong
        NSLog( @"cannot start real-time audio!" );
        // bail out
        return;
    }
}

@end
