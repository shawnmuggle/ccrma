//
//  WOInstrument.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Stk.h"
#import "SoundGrain.h"

@interface WOInstrument : NSObject {
    stk::StkFrames data;

    NSMutableDictionary* grainsDict;
    NSMutableArray* grainsArray;
    int grainIndex;
    
    float gain;
    float gainTarget;
}

@property (nonatomic, retain) NSMutableArray* grainsArray;

- (id) initWithFilename:(NSString*)filename;
- (void) setGrainWithId:(int)pointId ParamsX:(float)x Y:(float)y;
- (void) updateParamsWithBoundingBox:(CGRect)boundingBox;
- (void) tick;
- (void) tickAudio:(stk::StkFrames*)frames;
- (void) handleTouch:(CGPoint)loc withVelocity:(CGPoint)vel;

@end
