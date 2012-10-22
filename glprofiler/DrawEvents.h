//
//  DrawEvents.h
//  GLProfiler
//
//  Created by handley on 11/4/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Event.h"

@interface DrawArraysEvent : Event {
	struct DrawArraysPacked *data;
}
@property uint16_t mode;
@property uint32_t first, count;

@end
