//
//  GLState.h
//  GLProfiler
//
//  Created by handley on 11/23/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ClearColorEvent;
// It's important for the execution of the profiler that I shadow the driver state. This way, I can validate gl states before I send any information to the driver which might cause crashes.
@interface GLState : NSObject {
	ClearColorEvent *clearColorEvent;
}
+ (GLState *)state;

- (BOOL)validateClearColorEvent:(id *)ioValue error:(NSError **)outError;

@property(assign) ClearColorEvent *clearColorEvent;
@end
