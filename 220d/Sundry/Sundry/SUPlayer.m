//
//  SUPlayer.m
//  Sundry
//
//  Created by Michael Rotondo on 12/11/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUPlayer.h"

@implementation SUPlayer
@synthesize position;
@synthesize orientation;

- (id)initWithPosition:(GLKVector3)inPosition orientation:(GLKQuaternion)inOrientation
{
    self = [super init];
    if (self) {
        self.position = inPosition;
        self.orientation = inOrientation;
    }
    return self;
}

@end
