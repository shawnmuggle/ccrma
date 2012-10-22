//
//  MetaEvents.h
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Event.h"

@interface FrameEvent : Event {
	struct FramePacked *data;
}
@property uint32_t frameCount;
@end

@interface SurfaceEvent : Event {
	struct SurfacePacked *data;
}
@property uint16_t width, height;
@property uint8_t color, depth, stencil;
@end

@interface CaptureEvent : Event {
	//
}
@end

@interface GLStateEvent : Event {}
@end