//
//  GLState.m
//  GLProfiler
//
//  Created by handley on 11/23/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "GLState.h"
#import "ClearEvents.h"

static GLState *glstate;

@implementation GLState
+ (GLState *)state
{
	if (!glstate)
	{
		glstate = [[GLState alloc] init];
	}
	return glstate;
}
- (BOOL)validateClearColorEvent:(id *)ioValue error:(NSError **)outError
{
	if (!ioValue)
	{
		return NO;
	}
	
	//clearColorEvents are only invalid between begin/end, which we don't profile
	return YES;
}
@synthesize clearColorEvent;
@end
