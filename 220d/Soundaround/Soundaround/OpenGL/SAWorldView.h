//
//  SAWorldView.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/19/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <AppKit/AppKit.h>

#import <QuartzCore/QuartzCore.h>

#import "SAWorld.h"

@interface SAWorldView : NSOpenGLView

@property (nonatomic, retain) SAWorld *world;
@property (nonatomic, assign) CGRect viewport; // this is the size of the camera's window onto the world
@property (nonatomic, assign) CVDisplayLinkRef displayLink;

- (void)createDisplayLink;

- (void)drawRect:(NSRect)bounds;
- (void)draw;

- (void)moveCameraByVector:(CGPoint)vector;

- (void)keyUp:(NSEvent*)event;
- (void)keyDown:(NSEvent*)event;

@end

static CVReturn displayCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
                                CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext);