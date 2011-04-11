//
//  ES1Renderer.m
//  wold
//
//  Created by Michael Rotondo on 2/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "ES1Renderer.h"

#import "glu.h"

@implementation ES1Renderer

@synthesize state;

// Create an OpenGL ES 1.1 context
- (id)init
{
    self = [super init];
    if (self)
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

        if (!context || ![EAGLContext setCurrentContext:context])
        {
            [self release];
            return nil;
        }

        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &defaultFramebuffer);
        glGenRenderbuffersOES(1, &colorRenderbuffer);
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, 768, 1024);
        
        GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) ;
        if(status != GL_FRAMEBUFFER_COMPLETE_OES) {
            NSLog(@"failed to make complete framebuffer object %x", status);
        }
        
        
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
        
        glDepthFunc(GL_LEQUAL);
        glClearDepthf(1.0);
        glCullFace(GL_BACK);
        
        glDisable(GL_DITHER);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_FOG);
        glDisable(GL_TEXTURE_2D);
    }

    return self;
}

- (void)render
{
    
    // This application only creates a single context which is already set current at this point.
    // This call is redundant, but needed if dealing with multiple contexts.
    [EAGLContext setCurrentContext:context];

    // This application only creates a single default framebuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple framebuffers.
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    
    glViewport(0, 0, backingWidth, backingHeight);
    glGetIntegerv( GL_VIEWPORT, viewport );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    MoGfx::perspective( 45.0, backingWidth/(GLfloat)backingHeight, 1.0, 100 );
    
    glGetFloatv( GL_PROJECTION_MATRIX, projection );
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    MoGfx::lookAt( 0, 0, 0, 0, 0, -1, 0, 1, 0 );
    
    glPushMatrix();

    glGetFloatv( GL_MODELVIEW_MATRIX, modelview );
    
    [self.state render];

    glPopMatrix();
    
    // This application only creates a single color renderbuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple renderbuffers.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{	
    // Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
    {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }

    return YES;
}

- (void)processTouchLoc:(CGPoint)touchLoc
{    
    Vector3D nearPoint;
    Vector3D farPoint;
    
    gluUnProject((GLfloat)touchLoc.x, (GLfloat)touchLoc.y , 0.6, 
                 modelview, projection, viewport, 
                 (GLfloat*)&nearPoint.x, (GLfloat*)&nearPoint.y, (GLfloat*)&nearPoint.z);
    gluUnProject((GLfloat)touchLoc.x, (GLfloat)touchLoc.y , 0.9, 
                 modelview, projection, viewport, 
                 (GLfloat*)&farPoint.x, (GLfloat*)&farPoint.y, (GLfloat*)&farPoint.z);

    // TOTAL HACK, dunno why the points are ending up with flipped y coordinates from the actual models.
    nearPoint.y *= -1;
    farPoint.y *= -1;
    
    Vector3D ray = farPoint - nearPoint;
    [self.state handleTouchRay:ray fromPoint:nearPoint];
}

// TODO: Unify this and the above into a generic unproject function (include the duplicated code in WOPlanet to find the actual point on the surface of the planet)
- (void)processTapLoc:(CGPoint)touchLoc
{
    Vector3D nearPoint;
    Vector3D farPoint;
    
    gluUnProject((GLfloat)touchLoc.x, (GLfloat)touchLoc.y , 0.6, 
                 modelview, projection, viewport, 
                 (GLfloat*)&nearPoint.x, (GLfloat*)&nearPoint.y, (GLfloat*)&nearPoint.z);
    gluUnProject((GLfloat)touchLoc.x, (GLfloat)touchLoc.y , 0.9, 
                 modelview, projection, viewport, 
                 (GLfloat*)&farPoint.x, (GLfloat*)&farPoint.y, (GLfloat*)&farPoint.z);
    
    // TOTAL HACK, dunno why the points are ending up with flipped y coordinates from the actual models.
    nearPoint.y *= -1;
    farPoint.y *= -1;
    
    Vector3D ray = farPoint - nearPoint;
    
    [self.state handleTapRay:ray fromPoint:nearPoint];
}

- (void)processPressEnd
{
    [self.state processPressEnd];
}

- (void)processDrag:(UIPanGestureRecognizer*)gesture
{
    [self.state processDrag:gesture];
}

- (void)dealloc
{
    // Tear down GL
    if (defaultFramebuffer)
    {
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if (colorRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }

    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    [context release];
    context = nil;
    
    [super dealloc];
}

@end
