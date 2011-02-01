//
//  ES1Renderer.h
//  GOLIATH
//
//  Created by Michael Rotondo on 1/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "GOState.h"

@interface ES1Renderer : NSObject <ESRenderer>
{
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;
    
    GOState* state;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer, colorRenderbuffer;
}

- (void) drawSlingshot;
- (void) render;
- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer;

@end
