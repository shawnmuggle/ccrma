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
    int type;
    
    int treeId;
    
    int planetId;
}

@property float angle;
@property (nonatomic, retain) WOLSystem* lSystem;
@property int planetId;

// TODO: Unify tree & LSystem classes
- (id) initWithAngle:(float)newAngle andOrigin:(CGPoint)origin andType:(int)newType andFilename:(NSString*)filename;
- (void) tick;
- (void) tickAudio:(stk::StkFrames*)frames;
- (CALayer*) layer;
- (void) handleTouch:(CGPoint)loc withVelocity:(CGPoint)vel;
- (void) stopGrowing;
- (bool) isGrowing;
- (void) updateServer;
- (int) getNewTreeIdFromServer;

@end
