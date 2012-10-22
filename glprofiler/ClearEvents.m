//
//  ClearEvents.m
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ClearEvents.h"
#import "GLState.h"

@implementation ClearColorEvent
- (NSColor *)color
{
	CGFloat colorf[] = {
		((data->color >> 24) & 0xFF) / 255.99f,
		((data->color >> 16) & 0xFF) / 255.99f,
		((data->color >> 8) & 0xFF) / 255.99f,
		((data->color >> 0) & 0xFF) / 255.99f
	};
	return [NSColor colorWithColorSpace:[NSColorSpace genericRGBColorSpace] components:colorf count:4];
}
- (void)setColor:(NSColor *)color
{
	CGFloat colorf[4];
	if ([color numberOfComponents] == 4)
	{
		[color getComponents:colorf];
	}
	else if ([color numberOfComponents] == 3)
	{
		[color getComponents:colorf];
		colorf[3] = 1.0f;
	}
	data->color =
		(((int)(colorf[0] * 255.99f)) << 24) +
		(((int)(colorf[1] * 255.99f)) << 16) +
		(((int)(colorf[2] * 255.99f)) << 8) +
		(((int)(colorf[3] * 255.99f)) << 0);	
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Clear Color: (%08X)",data->color];
}
- (void)drawRect:(NSRect)rect
{
	CGFloat colorf[] = {
		((data->color >> 24) & 0xFF) / 255.99f,
		((data->color >> 16) & 0xFF) / 255.99f,
		((data->color >> 8) & 0xFF) / 255.99f,
		((data->color >> 0) & 0xFF) / 255.99f
	};
	glClearColor(colorf[0],colorf[1],colorf[2],colorf[3]);
}
- (NSString *)viewName
{
	return @"ClearColorEvent";
}
@end

@implementation ClearEvent

+ (NSColor *)defaultClearColor
{
	return [NSColor colorWithDeviceRed:0 green:0 blue:0 alpha:0];
}
- (NSColor *)color
{
	return clearColorEvent ? [clearColorEvent color] : [ClearEvent defaultClearColor]; //if we try to draw, but we don't have a clear color, than use the default clear color
}
- (void)setColor:(NSColor *)color
{
	if (!clearColorEvent)
	{
		clearColorEvent = [[ClearColorEvent alloc] init]; //make a temporary clear color event
	}
	clearColorEvent.color = color;
}
- (uint32_t)mask
{
	return data->mask;
}
- (void)setMask:(uint32_t)mask
{
	data->mask = mask;
}
- (NSString *)_name
{
	NSMutableString *str = [NSMutableString stringWithString:@"Clear: "];
	if (self.mask & GL_COLOR_BUFFER_BIT)
	{
		[str appendString:@"COLOR"];
	}
	if (self.mask & GL_DEPTH_BUFFER_BIT)
	{
		if ([str length] != 0)
			[str appendString:@" | "];
		[str appendString:@"DEPTH"];
	}
	if (self.mask & GL_STENCIL_BUFFER_BIT)
	{
		if ([str length] != 0)
			[str appendString:@" | "];
		[str appendString:@"STENCIL"];
	}
	
	return str;
}
- (void)drawRect:(NSRect)rect
{
	glClear(self.mask);
}
- (BOOL)isDrawEvent
{
	return YES;
}
@end
