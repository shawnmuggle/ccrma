//
//  SASoundPath.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/29/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>
#include "OpenGL/gl.h"
#import "SASoundPath.h"
#import "SASoundPoint.h"

@interface SASoundPath ()
{
@private
    NSMutableArray *points;
}
@end

@implementation SASoundPath 

- (id)init
{
    self = [super init];
    if (self)
    {
        points = [NSMutableArray arrayWithCapacity:10];
    }
    return self;
}

- (void)addPoint:(SASoundPoint *)newPoint
{
    [points addObject:newPoint];
}

- (void)draw
{
    if (![points count])
        return;
    
    glBegin(GL_TRIANGLE_STRIP);
    float angle;
    SASoundPoint *prevPoint = [points objectAtIndex:0];
    SASoundPoint *nextPoint;
    for (int i = 0; i < [points count]; i++)
    {
        SASoundPoint *soundPoint = [points objectAtIndex:i];
        if (i < [points count] - 1)
        {
            nextPoint = [points objectAtIndex:i+1];
        }
        else if (i == [points count] - 1)
        {
            nextPoint = soundPoint;
        }
        angle = atan2f(nextPoint.position.y - prevPoint.position.y, nextPoint.position.x - prevPoint.position.x);

        glColor3f(1.0f, 1.0f, 1.0f);
        
        CGPoint position = soundPoint.position;
        CGPoint rotatedTop = CGPointMake(position.x + soundPoint.intensity * sinf(angle), position.y - soundPoint.intensity * cosf(angle));
        CGPoint rotatedBottom = CGPointMake(position.x - soundPoint.intensity * sinf(angle), position.y + soundPoint.intensity * cosf(angle));
        glVertex2f(rotatedTop.x, rotatedTop.y);
        glVertex2f(rotatedBottom.x, rotatedBottom.y);
    }
    glEnd();
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    
}

@end
