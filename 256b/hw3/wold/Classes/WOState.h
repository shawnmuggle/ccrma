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

@interface WOState : NSObject {

    NSMutableSet* planets;
    
}

@property (nonatomic, retain) NSMutableSet* planets;

- (void) render;
- (void) handleTouchRay:(Vector3D)ray fromPoint:(Vector3D)touchPoint;
- (void) processPressEnd;
- (void) handleTapRay:(Vector3D)ray fromPoint:(Vector3D)touchPoint;
- (void) processDrag:(UIPanGestureRecognizer*)gesture;

@end
