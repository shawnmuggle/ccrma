//
//  GoliathSynth.m
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "GOSynth.h"

#import "mo_audio.h"

#import "Stk.h"
#import "BlitSquare.h"
#import "Shakers.h"
#import "OnePole.h"
#import "ModalBar.h"
#import "Plucked.h"
#import "JCRev.h"

#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2

float g_freq = 220;
float g_phase = 0;

stk::Shakers* shaker;
stk::OnePole* onePole;

stk::ModalBar* bar;
stk::OnePole* onePoleTwo;

stk::Plucked* string;

stk::JCRev* reverb;

// the audio callback
void audioCallback(Float32* buffer, UInt32 frameSize, void* userData)
{
    GOState* state = (GOState*)userData;

    if (state.num_wall_impacts > 0) {
        shaker->noteOn(8.1757989156 * pow(2, 10/12.0), 1.0);
        state.num_wall_impacts = 0;
    }

    if (state.num_ball_impacts > 0) {
        bar->setFrequency(8.1757989156 * pow(2, 60/12.0));
        bar->strike(1.0);
        state.num_ball_impacts = 0;
    }
    
    if (state.stringFrequency > 0) {
        string->setFrequency(state.stringFrequency);
        string->pluck(1.0);
        state.stringFrequency = -1;
    }
    
    reverb->setEffectMix(state.reverb);
    
    for ( int i = 0; i < frameSize; i++ )
    {
        buffer[i * 2] = buffer[i * 2 + 1] = 10 * reverb->tick(onePole->tick(shaker->tick()) + onePoleTwo->tick(bar->tick()) + 0.1 * string->tick());
    } 
}

@implementation GOSynth
@synthesize state;

- (id) initWithState:(GOState*) newState
{
    if (self = [super init]) {
        self.state = newState;
        
        shaker = new stk::Shakers();
        shaker->noteOn(200.0, 1.0);
        
        onePole = new stk::OnePole();
        
        bar = new stk::ModalBar();
        
        onePoleTwo = new stk::OnePole();
        onePoleTwo->setPole(0.96);
        
        string = new stk::Plucked(20);

        reverb = new stk::JCRev();
        reverb->setEffectMix(0.0);
        
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
        result = MoAudio::start( audioCallback, state);
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

- (void)dealloc
{
    delete shaker;
    delete onePole;
    delete bar;
    delete onePoleTwo;
    delete reverb;
    [super dealloc];
}

@end
