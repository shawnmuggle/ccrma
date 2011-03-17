//
//  SoundGrain.h
//  WoldSoundTest
//
//  Created by Michael Rotondo on 3/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Stk.h"

@interface SoundGrain : NSObject {
@private
    stk::StkFrames* frames;
    
    float baseStart;
    float newBaseStart;
    
    float density;
    float newDensity;
    
    float doneFraction;
    
    float masterGain;
    
    uint startSamp;
    uint lengthSamps;
    uint attackSamps;
    uint releaseSamps;
    
    uint currentSamp;
}

@property float newBaseStart;
@property float newDensity;
@property float masterGain;

- (id)initWithRandomParamsAndFrames:(stk::StkFrames*)frames;
- (void) updateParams;
- (void) tickAudio:(stk::StkFrames*)outFrames;
- (void) updateValsInBoundingBox:(CGRect)box;

@end
