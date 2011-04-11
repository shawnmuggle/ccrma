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
@synthesize newBaseStart, newDensity, masterGain, on, densityOffset, lengthOffset;

- (id)initWithRandomParamsAndFrames:(stk::StkFrames*)data
{
    self = [super init];
    if (self) {
        on = NO;
        
        frames = data;
        
        currentSamp = 0;
        startSamp = 0;
        masterGain = 0;
        
        baseStart = 0.5;
        density = 0.9;
        
        [self updateParams];
    }
    
    return self;
}

- (void)updateParams
{
    // This modulates independently of point position
    if (self.lengthOffset < -0.04) self.lengthOffset = -0.04;
    lengthSamps = 500 + 10000 * self.lengthOffset + rand() % (500 + (int)(10000 * self.lengthOffset));
    
    //NSLog(@"Length offset is %f and lengthSamps is %d", self.lengthOffset, lengthSamps);
    //NSLog(@"Sample length is %lu", frames->size());
    
    if (lengthSamps >= frames->size()) lengthSamps = frames->size();
    
    float startVariability = 0.09;
    startSamp = (frames->size() - lengthSamps) * (baseStart + startVariability * (rand() / (float)RAND_MAX));
    
    int fractionLength = lengthSamps * 0.2;  // The most that the attack or release can be is 2x this fraction
    attackSamps = fractionLength + fractionLength * (rand() / (float)RAND_MAX);
    releaseSamps = fractionLength + fractionLength * (rand() / (float)RAND_MAX);
    
    //if (self.densityOffset < -density * 0.9) self.densityOffset = -density * 0.9;
    doneFraction = 1 / (density + self.densityOffset);
    
    //NSLog(@"Density offset is %f and doneFraction is %f", self.densityOffset, doneFraction);
    //NSLog(@"Restarting with start: %d length: %d", startSamp, lengthSamps);
}

- (void) updateValsInBoundingBox:(CGRect)box
{
    if (self.newBaseStart != 0 && box.size.height != 0.0) {
        baseStart = (self.newBaseStart + box.size.height / 2.0) / box.size.height;
        
        if (baseStart > 0.9) baseStart = 0.9; // This + startVariability (above) can never add up to more than 1
        if (baseStart < 0) baseStart = 0;
        
        self.newBaseStart = 0.0;
    }
    if (self.newDensity != 0.0 && box.size.width != 0.0) {
        density = 1.0 - self.newDensity / box.size.width;
        
        if (density > 0.9) density = 0.9;
        if (density < 0.1) density = 0.1;
        
        self.newDensity = 0.0;
    }
    
    //NSLog(@"Base start: %f, Density: %f", baseStart, density);
}

- (void) tickAudio:(stk::StkFrames*)outFrames
{
    self.densityOffset *= 0.999;    
    self.lengthOffset *= 0.999;
    
    Float32 gain;
    for (int i = 0; i < outFrames->size(); i++) {
        stk::StkFloat sample = 0.0;
        if (currentSamp < lengthSamps) {
            //NSLog(@"Grabbing at index: %d", startSamp + currentSamp);
            sample = (*frames)[startSamp + currentSamp];
        }
        
        gain = 1.0;
        
        if (currentSamp < attackSamps)
            gain = currentSamp / (Float32)attackSamps;
        else if (currentSamp > lengthSamps - releaseSamps)
            gain = 1 - (currentSamp - (lengthSamps - releaseSamps)) / (Float32) releaseSamps;
        
        currentSamp++;
        
        if (currentSamp >= lengthSamps * doneFraction) {
            //NSLog(@"HEYO");
            [self updateParams];
            currentSamp = 0;
        }
        
        (*outFrames)[i] += sample * gain * masterGain;
    }
}

- (void)dealloc
{
    [super dealloc];
}

@end
