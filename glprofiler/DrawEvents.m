//
//  DrawEvents.m
//  GLProfiler
//
//  Created by handley on 11/4/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "DrawEvents.h"
#import "GLState.h"

@implementation DrawArraysEvent
- (uint16_t)mode
{
	return data->mode;
}
- (void)setMode:(uint16_t)mode
{
	data->mode = mode;
}
- (uint32_t)first
{
	return data->first;
}
- (void)setFirst:(uint32_t)first
{
	data->first = first;
}
- (uint32_t)count
{
	return data->count;
}
- (void)setCount:(uint32_t)count
{
	data->count = count;
}
- (NSString *)_name
{
	const char *modeString;
	switch (self.mode)
	{
	case GL_TRIANGLES:
		modeString = "Triangles";
		break;
	case GL_QUADS:
		modeString = "Quads";
		break;
	default:
		modeString = "Unknown";
		break;
	}
	
	return [NSString stringWithFormat:@"Draw %s: %u verts off: %u", modeString, self.count, self.first];
}
- (void)drawRect:(NSRect)rect
{
	glDrawArrays(self.mode, self.first, self.count);
}
- (BOOL)isDrawEvent
{
	return YES;
}
@end
