//
//  ProfileOpenGLView.h
//
//  Created by handley on 8/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class SurfaceEvent, ProfileOpenGLViewController;
@interface ProfileOpenGLView : NSOpenGLView {
	IBOutlet NSArrayController *controller;
	NSArray *cachedEvents;
	
	SurfaceEvent *cachedSurfaceEvent;
	BOOL needsInit;
}

@property(retain) NSArray *cachedEvents;
@property(retain) SurfaceEvent *cachedSurfaceEvent;
@property BOOL needsInit;
@end
