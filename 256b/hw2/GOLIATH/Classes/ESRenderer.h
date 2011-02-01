//
//  ESRenderer.h
//  GOLIATH
//
//  Created by Michael Rotondo on 1/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

#import "GOState.h"

@protocol ESRenderer <NSObject>

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

@property (nonatomic, retain) GOState* state;

@end
