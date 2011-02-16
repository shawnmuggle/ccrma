//
//  ES1Renderer.h
//  wold
//
//  Created by Michael Rotondo on 2/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "WOState.h"
#import "WOLSystem.h"

@interface ES1Renderer : NSObject <ESRenderer>
{
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
    
    WOState* state;
    
    GLfloat modelview[16];
    GLfloat projection[16];
    GLint viewport[4];
}

@property (nonatomic, retain) WOState* state;

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;
- (void)processTouchLoc:(CGPoint)touchLoc;
- (void)processDrag:(UIPanGestureRecognizer*)gesture;

@end
