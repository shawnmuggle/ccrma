//
//  SAWorldView.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/19/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <AppKit/AppKit.h>

#import <QuartzCore/QuartzCore.h>

@class SAWorld;
@class SAAudioManager;

@interface SAWorldView : NSOpenGLView

@property (nonatomic, retain) SAWorld *world;
@property (nonatomic, retain) SAAudioManager *audioManager;
@property (nonatomic, assign) CGRect viewport; // this is the size of the camera's window onto the world
@property (nonatomic, assign) CVDisplayLinkRef displayLink;

- (void)createDisplayLink;

- (void)drawRect:(NSRect)bounds;
- (void)draw;

- (void)setViewportToTrackAvatar;
- (void)setViewportSize:(CGSize)size;
- (void)positionViewportAt:(CGPoint)position;

- (void)keyUp:(NSEvent*)event;
- (void)keyDown:(NSEvent*)event;

float lerp(float a, float b, float t);
CGPoint CGPointLerp(const CGPoint &a, const CGPoint &b, float t);
CGSize CGSizeLerp(const CGSize &a, const CGSize &b, float t);

@end

static CVReturn displayCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
                                CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext);