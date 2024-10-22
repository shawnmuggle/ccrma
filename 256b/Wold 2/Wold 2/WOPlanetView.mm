//
//  WOPlanetView.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOPlanetView.h"
#import <QuartzCore/QuartzCore.h>

@implementation WOPlanetView

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        CADisplayLink* link = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
        [link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (void) tick:(id)sender
{
    [state tick:sender];
    [self setNeedsDisplay];
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{

}
*/
- (void)dealloc
{
    [super dealloc];
}

@end
