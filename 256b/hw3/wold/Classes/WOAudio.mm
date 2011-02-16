//
//  WOAudio.m
//  wold
//
//  Created by Michael Rotondo on 2/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOAudio.h"
#import "WOLSystem.h"
#import "WOPlanet.h"

#import "BandedWG.h"

#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2

// the audio callback
void audioCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    WOState* state = (WOState*)userData;
    
    for ( int i = 0; i < frameSize; i++ )
    {
        Float32 sample = 0.0;
        //Float32 leftSample = 0.0;
        //Float32 rightSample = 0.0;
        
        WOPlanet* planet = [state.planets anyObject];
        
        for (WOLSystem* tree in planet.trees) {
            sample += [tree tickAudio];
            //            float offset = tree.origin.x / 786.0;
            //            leftSample += [tree tickAudio] * (1 - offset);
            //            rightSample += [tree tickAudio] * offset;
        }
        
//        buffer[i * 2] = leftSample;
//        buffer[i * 2 + 1] = rightSample;
        buffer[i * 2] = buffer[i * 2 + 1] = sample;
    } 
}

@implementation WOAudio

@synthesize state;//, instruments;

- (id) initWithState:(WOState*)newState
{
    self = [super init];
    if (self) {
        self.state = newState;
        
//        self.instruments = [NSMutableArray arrayWithCapacity:6];
//        stk::BandedWG* instrument;
//        for (int i = 0; i < 6; i++) {
//            instrument = new stk::BandedWG();
//            [self.instruments addObject:[NSValue valueWithPointer:instrument]];
//        }
        
        //NSLog(@"HIELLOELIEOH");
        
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
        result = MoAudio::start( audioCallback, self.state);
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
