//
//  GrainTouchView.m
//  WoldSoundTest
//
//  Created by Michael Rotondo on 3/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "GrainTouchView.h"


@implementation GrainTouchView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

- (void)dealloc
{
    [super dealloc];
}

@end
