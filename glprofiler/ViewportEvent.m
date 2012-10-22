//
//  ViewportEvent.m
//  GLProfiler
//
//  Created by handley on 11/4/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ViewportEvent.h"


@implementation ViewportEvent
- (int32_t)x
{
	return data->x;
}
- (void)setX:(int32_t)x
{
	data->x = x;
}
- (int32_t)y
{
	return data->y;
}
- (void)setY:(int32_t)y
{
	data->y = y;
}
- (uint32_t)width
{
	return data->width;
}
- (void)setWidth:(uint32_t)width
{
	data->width = width;
}
- (uint32_t)height
{
	return data->height;
}
- (void)setHeight:(uint32_t)height
{
	data->height = height;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Viewport: (%u,%u,%u,%u)", self.x, self.y, self.width, self.height];
}
- (void)drawRect:(NSRect)rect
{
	glViewport(self.x, self.y, self.width, self.height);
}
@end
