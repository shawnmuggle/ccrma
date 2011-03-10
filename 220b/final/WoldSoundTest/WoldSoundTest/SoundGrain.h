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
    
    uint startSamp;
    uint lengthSamps;
    uint attackSamps;
    uint releaseSamps;
    
    uint currentSamp;
}

- (id)initWithRandomParamsAndFrames:(stk::StkFrames*)frames;
- (Float32) tick;
- (BOOL) done;

@end
