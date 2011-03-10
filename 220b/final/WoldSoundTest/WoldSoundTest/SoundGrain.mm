//
//  SoundGrain.m
//  WoldSoundTest
//
//  Created by Michael Rotondo on 3/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "SoundGrain.h"

#import "Stk.h"

@implementation SoundGrain

- (id)initWithRandomParamsAndFrames:(stk::StkFrames*)data
{
    self = [super init];
    if (self) {
        frames = data;
        
        lengthSamps = rand() % 500;
        startSamp = rand() % (frames->size() - lengthSamps);
        attackSamps = rand() % 100;
        releaseSamps = rand() % 100;
        
        currentSamp = 0;
    }
    
    return self;
}

- (Float32) tick
{
    Float32 sample = startSamp + currentSamp;
    
    Float32 gain = 1.0;
    
    if (currentSamp < attackSamps)
        gain = currentSamp / (Float32)attackSamps;
    else if (currentSamp > lengthSamps - releaseSamps)
        gain = 1 - (currentSamp - (lengthSamps - releaseSamps)) / (Float32) releaseSamps;
    
    currentSamp++;
    return sample * gain;
}

- (BOOL) done
{
    return currentSamp >= lengthSamps * 1.3;
}

- (void)dealloc
{
    [super dealloc];
}

@end
