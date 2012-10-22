//
//  ArrayEvents.h
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Event.h"

@interface EnableClientStateEvent : Event {
	struct EnableClientStatePacked *data;
}
@property uint32_t array;
@end

@interface PointerEvent : Event {
	struct PointerPacked *data;
}
@property uint32_t size, type, stride;
@property uintptr_t pointer;
@end

@interface VertexPointerEvent : PointerEvent {
}
@end
@interface ColorPointerEvent : PointerEvent {
}
@end
@interface TexCoordPointerEvent : PointerEvent {
}
@end
@interface NormalPointerEvent : PointerEvent {
}
@end
