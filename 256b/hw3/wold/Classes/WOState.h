//
//  WOState.h
//  wold
//
//  Created by Michael Rotondo on 2/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AKTouchTracker.h"

#import "mo_gfx.h"

#import "WOCamera.h"

@interface WOState : NSObject {

    AKTouchTracker* growingTrees;
    NSMutableSet* trees;
    NSMutableSet* planets;
    
    WOCamera* camera;
}

@property (nonatomic, retain) AKTouchTracker* growingTrees;
@property (nonatomic, retain) NSMutableSet* trees;
@property (nonatomic, retain) NSMutableSet* planets;
@property (nonatomic, retain) WOCamera* camera;

- (void) addTouch:(UITouch*) touch;
- (void) removeTouch:(UITouch*) touch;
- (void) moveTouch:(UITouch*) touch;
- (void) tick;
- (void) transformModelviewMatrix;
- (void) render;
- (void) handleTouchRay:(Vector3D)ray fromPoint:(Vector3D)touchPoint;
- (void) processDrag:(UIPanGestureRecognizer*)gesture;

@end
