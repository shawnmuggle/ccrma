//
//  WOTree.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WOLSystem.h"
#import "Stk.h"

@interface WOTree : NSObject {
    
    float angle;
    WOLSystem* lSystem;
    
}

@property float angle;
@property (nonatomic, retain) WOLSystem* lSystem;

// TODO: Unify tree & LSystem classes
- (id) initWithAngle:(float)newAngle andOrigin:(CGPoint)origin andType:(int)type;
- (void) tick;
- (void) tickAudio:(stk::StkFrames*)frames;
- (CALayer*) layer;

@end
