//
//  SAWorldView.m
//  Soundaround
//
//  Created by Michael Rotondo on 10/19/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include "OpenGL/gl.h"
#import <CoreVideo/CVDisplayLink.h>
#import "SAWorldView.h"
#import "SAWorld.h"
#import "SAAvatar.h"
#import "SABall.h"
#import "SAAudioManager.h"
#import "SAWorldViewController.h"

@interface SAWorldView ()
@property (nonatomic, assign) CGSize minViewportSize;

@end

@implementation SAWorldView
@synthesize world;
@synthesize viewport;
@synthesize displayLink;
@synthesize minViewportSize;
@synthesize audioManager;
@synthesize controller;
@synthesize followingBall;

-(BOOL)acceptsFirstResponder { return YES; }
-(BOOL)becomeFirstResponder  { return YES; }
-(BOOL)resignFirstResponder  { return YES; }

- (void)awakeFromNib
{
    self.world = [[SAWorld alloc] init];
    float aspectRatio = self.bounds.size.height / self.bounds.size.width;
    float minViewportWidth = 30; // in meters
    self.minViewportSize = CGSizeMake(minViewportWidth, minViewportWidth * aspectRatio);
    self.viewport = CGRectMake(0, 0, self.minViewportSize.width, self.minViewportSize.height);
    [self createDisplayLink];

    self.audioManager = [SAAudioManager audioManagerWithWorld:self.world];

    self.controller.world = self.world;
    self.nextResponder = self.controller;
    
    [[self window] setAcceptsMouseMovedEvents:YES];
}

- (void)dealloc
{
    CVDisplayLinkRelease(displayLink);
}

- (void)createDisplayLink
{
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(displayLink, &displayCallback, (__bridge void*) self);
    
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
    @autoreleasepool {
        [(__bridge SAWorldView*)displayLinkContext setNeedsDisplay:YES];
    }
    return kCVReturnSuccess;
}

- (void)drawRect:(NSRect)bounds
{
    [self draw];
}

- (void)draw
{
    if (!self.world.editMode)
    {
        if (!self.followingBall)
        {
            [self setViewportToTrackAvatar];
        }
        else
        {
            [self setViewportToTrackBall];
        }
    }
    else
    {
        [self setViewportToTrackMouse];
    }
    
    NSOpenGLContext *currentContext = [self openGLContext];
    [currentContext makeCurrentContext];
    
    // must lock GL context because display link is threaded
    CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(self.viewport.origin.x, self.viewport.origin.x + self.viewport.size.width,
            self.viewport.origin.y, self.viewport.origin.y + self.viewport.size.height,
            -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, self.bounds.size.width, self.bounds.size.height);

    glPushMatrix();

    [self.world draw];

    glPopMatrix();
    
    [currentContext flushBuffer];
    
    CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
}

- (void)setViewportToTrackAvatar
{
    CGPoint avatarPosition = self.world.avatar.position;
    float viewportSizeMultiplier = self.world.avatar.speed * 0.3;
    CGSize newViewportSize = CGSizeMake(self.minViewportSize.width + viewportSizeMultiplier, 
                                        self.minViewportSize.height + viewportSizeMultiplier);
    [self setViewportSize:newViewportSize];
    
    [self setViewportToTrackPoint:avatarPosition withThreshold:0.4];
}

- (void)setViewportToTrackBall
{
    CGPoint ballPosition = self.world.ball.position;
    float viewportSizeMultiplier = self.world.ball.speed * 0.3;
    CGSize newViewportSize = CGSizeMake(self.minViewportSize.width + viewportSizeMultiplier, 
                                        self.minViewportSize.height + viewportSizeMultiplier);
    [self setViewportSize:newViewportSize];
#warning FIXME to track the ball position vertically
    [self setViewportToTrackPoint:ballPosition withThreshold:0.4];
}

- (void)setViewportToTrackMouse
{
    float viewportSizeMultiplier = 2.0;
    CGSize newViewportSize = CGSizeMake(self.minViewportSize.width * viewportSizeMultiplier, 
                                        self.minViewportSize.height * viewportSizeMultiplier);
    [self setViewportSize:newViewportSize];

    CGPoint mousePos = [NSEvent mouseLocation];
    CGRect windowPosRect = [[self window] convertRectFromScreen:CGRectMake(mousePos.x, mousePos.y, 0, 0)];
    CGPoint viewPos = [self convertPoint:windowPosRect.origin fromView:nil];    
    CGPoint worldPos = CGPointMake(self.viewport.origin.x + (viewPos.x / self.bounds.size.width) * self.viewport.size.width,
                                   self.viewport.origin.y + (viewPos.y / self.bounds.size.height) * self.viewport.size.height);
    
    [self setViewportToTrackPoint:worldPos withThreshold:0.1];
}


- (void)setViewportToTrackPoint:(CGPoint)point withThreshold:(float)movementThreshold
{
    if (point.x < self.viewport.origin.x + movementThreshold * self.viewport.size.width)
    {
        [self positionViewportAt:CGPointMake(point.x - movementThreshold * self.viewport.size.width, 
                                             self.viewport.origin.y)];
    }
    if (point.x > self.viewport.origin.x + self.viewport.size.width - movementThreshold * self.viewport.size.width)
    {
        [self positionViewportAt:CGPointMake(point.x - (1.0 - movementThreshold) * self.viewport.size.width, self.viewport.origin.y)];
    }
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

CGPoint CGPointLerp(const CGPoint &a, const CGPoint &b, float t)
{
    CGPoint p = CGPointMake(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
    return p;
}

CGSize CGSizeLerp(const CGSize &a, const CGSize &b, float t)
{
    CGSize s = CGSizeMake(lerp(a.width, b.width, t), lerp(a.height, b.height, t));
    return s;
}

- (void)setViewportSize:(CGSize)size
{
    CGSize newSize = CGSizeLerp(self.viewport.size, size, 0.1);
    self.viewport = CGRectMake(self.viewport.origin.x, self.viewport.origin.y, newSize.width, newSize.height);
}

- (void)positionViewportAt:(CGPoint)position
{
    self.viewport = CGRectMake(position.x, position.y, 
                               self.viewport.size.width, self.viewport.size.height);
}

- (CGPoint)worldPosForEvent:(NSEvent *)event
{
    CGPoint windowPos = [event locationInWindow];
    CGPoint viewPos = [self convertPoint:windowPos fromView:nil];
    CGPoint worldPos = CGPointMake(self.viewport.origin.x + (viewPos.x / self.bounds.size.width) * self.viewport.size.width,
                                   self.viewport.origin.y + (viewPos.y / self.bounds.size.height) * self.viewport.size.height);
    return worldPos;
}

@end
