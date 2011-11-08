//
//  SABallGuide.mm
//  Soundaround
//
//  Created by Michael Rotondo on 11/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>
#include "OpenGL/gl.h"
#import "SABallGuide.h"
#import "SABallGuidePoint.h"
#import "SAWorld.h"
#import "SABall.h"

float CGPointMagnitudeSquared(CGPoint p);
float CGPointMagnitude(CGPoint p);
CGPoint CGPointSubtract(CGPoint p1, CGPoint p2);

float CGPointMagnitudeSquared(CGPoint p)
{
    return powf(p.x, 2) + powf(p.y, 2);
}

float CGPointMagnitude(CGPoint p)
{
    return sqrtf(CGPointMagnitudeSquared(p));
}

CGPoint CGPointSubtract(CGPoint p1, CGPoint p2)
{
    return CGPointMake(p1.x - p2.x, p1.y - p2.y);
}

struct Movement {
    CGPoint start;
    CGPoint end;
};

@interface SABallGuide ()
{
    NSMutableArray *points;
    NSDate *lastPointTime;

    SABall *activeBall;
    NSDate *timeActivated;
    NSTimeInterval timeSpentActive;
    
    float phase;
    
    Movement lastBallMovement;
}

@end

@implementation SABallGuide 

- (id)init
{
    self = [super init];
    if (self)
    {
        points = [NSMutableArray arrayWithCapacity:10];
    }
    return self;
}

- (void)addPoint:(SABallGuidePoint *)newPoint
{
    NSDate *newTime = [NSDate date];
    
    SABallGuidePoint *lastPoint = [points lastObject];
    if ( lastPoint )
        lastPoint.timeToNextPoint = [newTime timeIntervalSinceDate:lastPointTime];
    
    [points addObject:newPoint];
    lastPointTime = newTime;
}

- (BOOL)isValid
{
    BOOL bigEnough = [points count] > 2;
    return bigEnough;
}

- (BOOL)shouldActivateInWorld:(SAWorld *)world
{
    if (![self isValid])
        return NO;
    
    SABallGuidePoint *firstPoint = [points objectAtIndex:0];

    return [firstPoint isBallClose:world.ball];;
}

- (void)activateWithBall:(SABall *)ball
{
    timeActivated = [NSDate date];
    
    activeBall = ball;
    activeBall.currentGuide = self;
    
    timeSpentActive = 0.0;
    
    activeBall.physicsBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    activeBall.physicsBody->SetGravityScale(0.0f);
}

- (void)updateInWorld:(SAWorld *)world withTimeInterval:(NSTimeInterval)timeInterval
{
    if (!activeBall || ![self isValid])
        return;
    
    timeSpentActive += timeInterval;
    NSTimeInterval currentSegmentStartTime = 0.0;
    SABallGuidePoint *currentSegmentStartPoint, *currentSegmentEndPoint;

    for (int i = 0; i < [points count] - 1; i++)
    {
        SABallGuidePoint *guidePoint = [points objectAtIndex:i];
        currentSegmentStartPoint = guidePoint;
        if (currentSegmentStartTime + guidePoint.timeToNextPoint > timeSpentActive)
        {
            currentSegmentEndPoint = [points objectAtIndex:i+1];
            break;
        }
        else
        {
            currentSegmentStartTime += guidePoint.timeToNextPoint;
        }
    }
    
    // TODO: This is the shittiest way of moving the ball between the points possible! Make it cooler
    float segmentCompletionPercent = (timeSpentActive - currentSegmentStartTime) / currentSegmentStartPoint.timeToNextPoint;
    CGPoint currentSegmentVector = CGPointSubtract(currentSegmentEndPoint.position, currentSegmentStartPoint.position);
    b2Vec2 partialSegmentVector = segmentCompletionPercent * b2Vec2(currentSegmentVector.x, currentSegmentVector.y);
    b2Vec2 position = b2Vec2(currentSegmentStartPoint.position.x, currentSegmentStartPoint.position.y) + partialSegmentVector;

    b2Vec2 start = activeBall.physicsBody->GetWorldCenter();
    activeBall.physicsBody->SetTransform(position, 0.0f);

    Movement m;
    m.start = CGPointMake(start.x, start.y);
    m.end = CGPointMake(position.x, position.y);
    lastBallMovement = m;
    
//    NSLog(@"Last ball movement set to : %f, %f => %f, %f", m.start.x, m.start.y, m.end.x, m.end.y);
    
    SABallGuidePoint *lastPoint = [points lastObject];
    if (currentSegmentEndPoint == lastPoint && [lastPoint isBallClose:activeBall])
    {
//        SABallGuidePoint *secondToLastPoint = [points objectAtIndex:[points count] - 2];
//        b2Vec2 vector = b2Vec2(lastPoint.position.x, lastPoint.position.y) - b2Vec2(secondToLastPoint.position.x, secondToLastPoint.position.y);
//        float length = vector.Length();
//        vector.Normalize();
//        activeBall.physicsBody->SetLinearVelocity((length / secondToLastPoint.timeToNextPoint) * vector);
        [self deactivate];
        return;
    }
}

- (void)deactivate
{
    activeBall.physicsBody->SetGravityScale(1.0f);
    activeBall.currentGuide = nil;
    activeBall = nil;
}

- (void)draw
{
    if (![points count])
        return;
    
    glBegin(GL_TRIANGLE_STRIP);
    float angle, intensity, width;
    SABallGuidePoint *prevPoint = [points objectAtIndex:0];
    SABallGuidePoint *nextPoint;
    for (int i = 0; i < [points count]; i++)
    {
        SABallGuidePoint *soundPoint = [points objectAtIndex:i];
        if (i < [points count] - 1)
        {
            nextPoint = [points objectAtIndex:i+1];
        }
        else if (i == [points count] - 1)
        {
            nextPoint = soundPoint;
        }
        angle = atan2f(nextPoint.position.y - prevPoint.position.y, nextPoint.position.x - prevPoint.position.x);
        intensity = CGPointMagnitude(CGPointSubtract(nextPoint.position, soundPoint.position));
        width = 0.1f + (intensity / 20);
        prevPoint = soundPoint;
        
        glColor3f(1.0f, 1.0f, 1.0f);
        
        CGPoint position = soundPoint.position;
        CGPoint rotatedTop = CGPointMake(position.x + (width / 2.0f) * sinf(angle), position.y - (width / 2.0f) * cosf(angle));
        CGPoint rotatedBottom = CGPointMake(position.x - (width / 2.0f) * sinf(angle), position.y + (width / 2.0f) * cosf(angle));
        glVertex2f(rotatedTop.x, rotatedTop.y);
        glVertex2f(rotatedBottom.x, rotatedBottom.y);

    }
    glEnd();
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    if (!activeBall)
        return;
    
    int numChannels = 2;
    
    Movement m = lastBallMovement;
        
    float frequency;
    
    float amplitude = 1.0f;
    
    for (int i = 0; i < numBufferFrames; i++)
    {
        float percent = (float) i / (numBufferFrames - 1);
        frequency = 100.0f * (m.start.y + percent * (m.end.y - m.start.y));
        
        float noiseSample = 0.0f;  //(rand() / (float)RAND_MAX);
        float sineSample = sin(phase);
        float envelopedSample = (noiseSample + sineSample) * amplitude;
        for (int j = 0; j < numChannels; j++)
        {
            output[i * numChannels + j] += envelopedSample;
        }
        phase += (frequency * 2 * M_PI) / 44100;
        while (phase > 2 * M_PI)
        {
            phase -= 2 * M_PI;
        }
    }
}

@end
