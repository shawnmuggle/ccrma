//
//  SAWorldView.m
//  Soundaround
//
//  Created by Michael Rotondo on 10/19/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SAWorldView.h"
#include "OpenGL/gl.h"

@implementation SAWorldView
@synthesize world;
@synthesize viewport;
@synthesize displayLink;

- (void)awakeFromNib
{
    self.world = [SAWorld world];
    float aspectRatio = self.bounds.size.height / self.bounds.size.width;
    float viewportWidth = 30; // in meters
    self.viewport = CGRectMake(0, 0, viewportWidth, viewportWidth * aspectRatio);
    [self createDisplayLink];
}

- (void)dealloc
{
    CVDisplayLinkRelease(displayLink);
    [world release];
    [super dealloc];
}

-(BOOL)acceptsFirstResponder { return YES; }
-(BOOL)becomeFirstResponder  { return YES; }
-(BOOL)resignFirstResponder  { return YES; }

- (void)createDisplayLink
{
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(displayLink, &displayCallback, self);
    
    // Set the display link for the current renderer
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    // Activate the display link
    CVDisplayLinkStart(displayLink);
}

static CVReturn displayCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
//    [(SAWorldView*)displayLinkContext draw];
    [(SAWorldView*)displayLinkContext setNeedsDisplay:YES];
    return kCVReturnSuccess;
}

- (void)drawRect:(NSRect)bounds
{
    [self draw];
}

- (void)draw
{
    NSOpenGLContext *currentContext = [self openGLContext];
    [currentContext makeCurrentContext];
    
    // must lock GL context because display link is threaded
    CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, self.viewport.size.width,
            0, self.viewport.size.height,
            -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, self.bounds.size.width, self.bounds.size.height);

    glPushMatrix();
    glTranslatef(-self.viewport.origin.x, -self.viewport.origin.y, 0.0);

    [self.world draw];

    glPopMatrix();
    
    [currentContext flushBuffer];
    
    CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
}

- (void)moveCameraByVector:(CGPoint)vector
{
    self.viewport = CGRectMake(self.viewport.origin.x + vector.x, self.viewport.origin.y + vector.y, 
                               self.viewport.size.width, self.viewport.size.height);
}

- (void)mouseDown:(NSEvent *)theEvent {
    CGPoint windowPos = [theEvent locationInWindow];
    CGPoint viewPos = [self convertPoint:windowPos fromView:nil];
    CGPoint worldPos = CGPointMake(self.viewport.origin.x + (viewPos.x / self.bounds.size.width) * self.viewport.size.width,
                                   self.viewport.origin.y + (viewPos.y / self.bounds.size.height) * self.viewport.size.height);
    NSLog(@"World pos of click: %@", NSStringFromPoint(worldPos));
    [self.world setTargetPoint:worldPos];
}

-(void)keyUp:(NSEvent*)event
{
    NSLog(@"Key released: %@", event);
}

-(void)keyDown:(NSEvent*)event
{   
    CGPoint movementVector = CGPointMake(0, 0);
    float movementAmount = 1.0;
    
    // I added these based on the addition to your question :)
    switch( [event keyCode] ) {
        case 124:       // right arrow
            movementVector = CGPointMake(movementVector.x + movementAmount, movementVector.y);
            break;
        case 123:       // left arrow
            movementVector = CGPointMake(movementVector.x - movementAmount, movementVector.y);
            break;
        case 126:       // up arrow
        case 125:       // down arrow
        default:
            NSLog(@"Key pressed: %@", event);
            break;
    }
    
    [self moveCameraByVector:movementVector];
}

@end
