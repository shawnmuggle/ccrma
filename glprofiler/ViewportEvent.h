//
//  ViewportEvent.h
//  GLProfiler
//
//  Created by handley on 11/4/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Event.h"

@interface ViewportEvent : Event {
	struct ViewportPacked *data;
}
@property int32_t x,y;
@property uint32_t width,height;
@end
