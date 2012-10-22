//
//  MetaEvents.m
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MetaEvents.h"


@implementation FrameEvent
- (uint32_t)frameCount
{
	return data->frameCount;
}
- (void)setFrameCount:(uint32_t)frameCount
{
	data->frameCount = frameCount;
}
- (BOOL)isScoped
{
	return self.msgID == etFRAME_START;
}
- (BOOL)isEndScope
{
	return self.msgID == etFRAME_END;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Frame: %u",data ? data->frameCount : 0];
}
@end

@implementation SurfaceEvent
- (uint16_t)width
{
	return data->width;
}
- (void)setWidth:(uint16_t)width
{
	data->width = width;
}
- (uint16_t)height
{
	return data->height;
}
- (void)setHeight:(uint16_t)height
{
	data->height = height;
}
- (uint8_t)color
{
	return data->color;
}
- (void)setColor:(uint8_t)color
{
	data->color = color;
}
- (uint8_t)depth
{
	return data->depth;
}
- (void)setDepth:(uint8_t)depth
{
	data->depth = depth;
}
- (uint8_t)stencil
{
	return data->stencil;
}
- (void)setStencil:(uint8_t)stencil
{
	data->stencil = stencil;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Surface: %uC%uD%uS %ux%u",self.color, self.depth, self.stencil, self.width, self.height];
}
@end

@implementation CaptureEvent
- (NSString *)_name
{
	if (self.time == 0 && self.children)
	{
		self.time = ((Event *)self.children.lastObject).time;
	}
	
	CFTimeInterval ti = self.time / 1e6;
	
	return [NSDateFormatter localizedStringFromDate:[NSDate dateWithTimeIntervalSince1970:ti] dateStyle:NSDateFormatterShortStyle timeStyle:NSDateFormatterShortStyle];
}
- (BOOL)isScoped
{
	return self.msgID == etCAPTURE;
}
- (BOOL)isEndScope
{
	return self.msgID == etSTOP_CAPTURE;
}
@end

@implementation GLStateEvent
- (NSString *)_name
{
	return @"Initial GL State";
}
@end
