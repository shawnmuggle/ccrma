//
//  ClearEvents.h
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Event.h"

@interface ClearColorEvent : Event {
	struct ClearColorPacked *data;
}
@property(copy) NSColor *color;
@end

@interface ClearEvent : Event {
	struct ClearPacked *data;
	
	ClearColorEvent *clearColorEvent;
}
+ (NSColor *)defaultClearColor;

@property(copy) NSColor *color;
@property(assign) uint32_t mask;
@end
