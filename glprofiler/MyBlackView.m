//
//  MyBlackView.m
//
//  Created by handley on 8/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MyBlackView.h"

@implementation MyBlackView
- (void)drawRect:(NSRect)dirtyRect
{
	[[NSColor blackColor] set];
	NSRectFill(dirtyRect);
}
- (BOOL)isOpaque
{
	return YES;
}
@end
