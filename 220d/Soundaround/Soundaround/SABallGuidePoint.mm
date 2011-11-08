//
//  SABallGuidePoint.mm
//  Soundaround
//
//  Created by Michael Rotondo on 11/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>
#import "SABallGuidePoint.h"
#import "SABall.h"

@implementation SABallGuidePoint
@synthesize position;
@synthesize timeToNextPoint;

- (id)initWithPosition:(CGPoint)inPosition
{
    self = [super init];
    if (self)
    {
        position = inPosition;
    }
    return self;
}

- (BOOL)isBallClose:(SABall *)ball
{
    float closenessThreshold = 1;
    b2Vec2 vectorToBall = ball.physicsBody->GetWorldCenter() - b2Vec2(position.x, position.y);
    BOOL ballClose = vectorToBall.Length() < closenessThreshold;
    return ballClose;
}

@end
