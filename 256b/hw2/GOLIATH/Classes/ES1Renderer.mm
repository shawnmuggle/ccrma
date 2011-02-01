//
//  ES1Renderer.m
//  GOLIATH
//
//  Created by Michael Rotondo on 1/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "ES1Renderer.h"

#import "mo_gfx.h"

GLuint g_texture[3];

@interface ES1Renderer ()

- (void) drawRingAtPoint:(CGPoint)loc withRadius:(GLfloat)radius andAlpha:(float)alpha;

@end


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
        // enable texture mapping
        glEnable( GL_TEXTURE_2D );
        // enable blending
        glEnable( GL_BLEND );
        // blend function
        //glBlendFunc( GL_ONE, GL_ZERO );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        
        // generate texture name
        glGenTextures( 2, &g_texture[0] );

        // bind the texture
        glBindTexture( GL_TEXTURE_2D, g_texture[0] );
        // setting parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        // load the texture
        MoGfx::loadTexture( @"glow_texture", @"png" );
        
        // bind the texture
        glBindTexture( GL_TEXTURE_2D, g_texture[1] );
        // setting parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        // load the texture
        MoGfx::loadTexture( @"ring_texture", @"png" );

        // bind the texture
        glBindTexture( GL_TEXTURE_2D, g_texture[2] );
        // setting parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        // load the texture
        MoGfx::loadTexture( @"arrow_texture", @"png" );
        
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &defaultFramebuffer);
        glGenRenderbuffersOES(1, &colorRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
    }

    return self;
}

- (void) drawRingAtPoint:(CGPoint)loc withRadius:(GLfloat)radius andAlpha:(float)alpha
{
    // Enable drawing states
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    
    static const GLfloat circleTexCoords[] = {
        0, 0,
        0, 1,
        1, 1,
        1, 0
    };
    
    GLfloat circleVertices[] = {
        -radius,  -radius,
        -radius,  radius,
        radius,   radius,
        radius,   -radius,
    };
    
    const GLubyte circleColors[] = {
        255, 229, 109, 255 * alpha,
        255, 229, 109, 255 * alpha,
        255, 229, 109, 255 * alpha,
        255, 229, 109, 255 * alpha,
    };
    
    glPushMatrix();
    glTranslatef(loc.x, loc.y, 0.0f);    
    glVertexPointer(2, GL_FLOAT, 0, circleVertices);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, circleColors);
    glTexCoordPointer(2, GL_FLOAT, 0, circleTexCoords);
    glBindTexture(GL_TEXTURE_2D, g_texture[1]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();

    // Cleanup - Disable client states
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);    
}

- (void) drawSlingshot
{
    // Get locations of slingshot-end touches
    UITouch* touch1 = [state.touches objectForKey:@"line_end_one"];
    UITouch* touch2 = [state.touches objectForKey:@"line_end_two"];
    CGPoint loc1 = [touch1 locationInView:touch1.view];
    CGPoint loc2 = [touch2 locationInView:touch2.view];

    // Draw the circles
    
    GLfloat radius = 50.0f;
    [self drawRingAtPoint:loc1 withRadius:radius andAlpha:1.0];
    [self drawRingAtPoint:loc2 withRadius:radius andAlpha:1.0];

    // DRAW THE LINE
    GLfloat line_width = 10;
    
    // Enable drawing states
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    
    static const GLfloat lineTexCoords[] = {
        0.2, 0.2,
        0.2, 0.8,
        0.8, 0.8,
        0.8, 0.2
    };
    
    static const GLubyte lineColors[] = {
        255, 229, 109, 255,
        255, 229, 109, 255,
        255, 229, 109, 255,
        255, 229, 109, 255,
    };
    
    CGPoint lastPoint = loc1;
    
    int i = 0;
    for (NSValue* pointValue in state.line_points) {
        Vector3D* pointVector = (Vector3D*)[pointValue pointerValue];
        CGPoint point; point.x = pointVector->x; point.y = pointVector->y;
        
        Vector3D v1(loc1.x, loc1.y, 0);
        Vector3D v2(loc2.x, loc2.y, 0);
        
        Vector3D difference = v2 - v1;
        difference.normalize();
        GLfloat tmp = difference.y;
        difference.y = difference.x;
        difference.x = -tmp;
        
        NSNumber* offset = [state.line_offsets objectAtIndex:i];
        point.x += (difference * [offset floatValue]).x;
        point.y += (difference * [offset floatValue]).y;
        
        GLfloat line_length = sqrt(pow(point.x - lastPoint.x, 2) + pow(point.y - lastPoint.y, 2));
        GLfloat angle = (180 / (M_PI)) * atan2(lastPoint.y - point.y, lastPoint.x - point.x);

        GLfloat lineVertices[] = {
            0,  -line_width/2,
            0,  line_width/2,
            line_length, line_width/2,
            line_length, -line_width/2,
        };
        
        glPushMatrix();
        glTranslatef(point.x, point.y, 0);
        glRotatef(angle, 0, 0, 1);
        glVertexPointer(2, GL_FLOAT, 0, lineVertices);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, lineColors);
        glTexCoordPointer(2, GL_FLOAT, 0, lineTexCoords);
        glBindTexture(GL_TEXTURE_2D, g_texture[0]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();
        
        lastPoint = point;
        i++;
    }

    GLfloat line_length = sqrt(pow(loc2.x - lastPoint.x, 2) + pow(loc2.y - lastPoint.y, 2));
    GLfloat angle = (180 / (M_PI)) * atan2(lastPoint.y - loc2.y, lastPoint.x - loc2.x);

    //line_width += 10;
    
    GLfloat lineVertices[] = {
        0,  -line_width/2,
        0,  line_width/2,
        line_length, line_width/2,
        line_length, -line_width/2,
    };
    
    glPushMatrix();
    glTranslatef(loc2.x, loc2.y, 0);
    glRotatef(angle, 0, 0, 1);
    glVertexPointer(2, GL_FLOAT, 0, lineVertices);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, lineColors);
    glTexCoordPointer(2, GL_FLOAT, 0, lineTexCoords);
    glBindTexture(GL_TEXTURE_2D, g_texture[0]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();
    
    UITouch* pullTouch = [state.touches objectForKey:@"pull_touch"];
    if (pullTouch != nil) {
        Vector3D v1; v1.x = loc1.x; v1.y = loc1.y;
        Vector3D v2; v2.x = loc2.x; v2.y = loc2.y;
        Vector3D center = v1 + (v2 - v1) * 0.5;
        
        CGPoint pullTouchLoc = [pullTouch locationInView:pullTouch.view];
        Vector3D pullPoint; pullPoint.x = pullTouchLoc.x; pullPoint.y = pullTouchLoc.y;
        
        Vector3D arrowVector = center - pullPoint;
        GLfloat arrowLength = arrowVector.magnitude();
        GLfloat arrowWidth = 40;
        GLfloat arrowAngle = (180 / (M_PI)) * arrowVector.angleXY();
        
        GLfloat arrowVertices[] = {
            0,  -arrowWidth/2,
            0,  arrowWidth/2,
            arrowLength, arrowWidth/2,
            arrowLength, -arrowWidth/2,
        };
        
        static const GLfloat arrowTexCoords[] = {
            0, 0,
            0, 1,
            1, 1,
            1, 0
        };
        
        static const GLubyte arrowColors[] = {
            255, 229, 109, 100,
            255, 229, 109, 100,
            255, 229, 109, 100,
            255, 229, 109, 100,
        };
        
        glPushMatrix();
        glTranslatef(pullPoint.x, pullPoint.y, 0);
        glRotatef(arrowAngle, 0, 0, 1);
        glVertexPointer(2, GL_FLOAT, 0, arrowVertices);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, arrowColors);
        glTexCoordPointer(2, GL_FLOAT, 0, arrowTexCoords);
        glBindTexture(GL_TEXTURE_2D, g_texture[2]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();
    }
    
    // Cleanup - Disable client states
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

- (void)render
{
    // Replace the implementation of this method to do your own custom drawing

    static float transY = 0.0f;

    // This application only creates a single context which is already set current at this point.
    // This call is redundant, but needed if dealing with multiple contexts.
    [EAGLContext setCurrentContext:context];

    // This application only creates a single default framebuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple framebuffers.
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrthof(0, backingWidth, backingHeight, 0, 0, 10);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    transY += 0.075;

    if ([state slingshotPresent]) {
        [self drawSlingshot];
    }
      
    for (GOBall* ball in state.balls) {
        CGPoint ballPosition = CGPointMake(ball.position->x, ball.position->y); 
        [self drawRingAtPoint:ballPosition withRadius:ball.radius andAlpha:1 - (float)ball.age/ball.lifetime];
    }

    for (GOCollision* collision in state.collisions) {
        CGPoint collisionPosition = CGPointMake(collision.position->x, collision.position->y); 
        GLfloat radius = 100 * (collision.age / (float)collision.lifetime);
        [self drawRingAtPoint:collisionPosition withRadius:radius andAlpha:1 - (float)collision.age/collision.lifetime];
        [self drawRingAtPoint:collisionPosition withRadius:0.6 * radius andAlpha:0.8 * (1 - (float)collision.age/collision.lifetime)];
        [self drawRingAtPoint:collisionPosition withRadius:0.3 * radius andAlpha:0.5 * (1 - (float)collision.age/collision.lifetime)];
    }
    
    // This application only creates a single color renderbuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple renderbuffers.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
    
    [state tick];
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
