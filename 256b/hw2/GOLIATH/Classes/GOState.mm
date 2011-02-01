//
//  GoliathState.m
//  GOLIATH
//
//  Created by Michael Rotondo on 1/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "GOState.h"

#import "mo_accel.h"

int num_line_points = 101; // NEEDS TO BE ODD

void accelCallback( double x, double y, double z, void * data )
{
    GOState* state = (GOState*)data;
    float mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    if (state.shake && mag > 2) {
        [state clear];
    }
}

@interface GOState ()

- (void) bounceBallOffScreenEdges:(GOBall*) ball;
- (void) bounceBallOffBalls:(GOBall*) ball;
- (BOOL) ageAndDeleteBall:(GOBall*)ball;
- (BOOL) ageAndDeleteCollision:(GOCollision*)collision;
- (void) destroySlingshot;

@end


@implementation GOState
@synthesize touches;
@synthesize line_points;
@synthesize line_offsets;
@synthesize balls;
@synthesize num_wall_impacts;
@synthesize collisions;
@synthesize num_ball_impacts;
@synthesize stringFrequency;
@synthesize damping;
@synthesize reverb;
@synthesize shake;
@synthesize gravity;

- (id) init
{
    if (self = [super init]) {
        MoAccel::addCallback( accelCallback, self);
        
        touches = (NSMutableDictionary*)CFDictionaryCreateMutable(NULL,11,NULL,NULL);
        
        line_points = [[NSMutableArray alloc] init];
        line_offsets = [[NSMutableArray alloc] init];
        
        for (int i = 0; i < num_line_points; i++) {
            NSValue* pointValue = [NSValue valueWithPointer:new Vector3D()];
            [line_points insertObject:pointValue atIndex:i];
            
            [line_offsets insertObject:[NSNumber numberWithFloat:0] atIndex:i];
        }
        
        balls = [[NSMutableArray alloc] init];
        self.num_wall_impacts = 0;
    
        collisions = [[NSMutableArray alloc] init];
        self.num_ball_impacts = 0;
        
        tickCount = 0;
        stringMultiplier = 0;
        
        stringFrequency = -1;
        
        touchesPluckable = (NSMutableSet*)CFSetCreateMutable(NULL, 11, NULL);
    }
    return self;
}

- (void) clear
{
    [self.balls removeAllObjects];
}

- (void) addTouch:(UITouch*) touch
{
    if ([touches count] == 0) {
        [touches setObject:touch forKey:@"line_end_one"];
        stringMultiplier = 0;
    }
    else if ([touches count] == 1) {
        [touches setObject:touch forKey:@"line_end_two"];
        stringMultiplier = 0;
    }
    else if ([touches count] == 2) {
        UITouch* touch1 = [touches objectForKey:@"line_end_one"];
        CGPoint loc1 = [touch1 locationInView:touch1.view];
        Vector3D v1(loc1.x, loc1.y, 0);
        UITouch* touch2 = [touches objectForKey:@"line_end_two"];
        CGPoint loc2 = [touch2 locationInView:touch2.view];
        Vector3D v2(loc2.x, loc2.y, 0);
        
        Vector3D difference = v2 - v1;
        float difference_mag = difference.magnitude();
        difference.normalize();
        
        CGPoint touchPositionPoint = [touch locationInView:touch.view];
        Vector3D touchPosition(touchPositionPoint.x, touchPositionPoint.y, 0);
        Vector3D relativeTouchPosition(touchPosition.x - loc1.x, touchPosition.y - loc1.y, 0);

        float projectionCoefficient = (difference * relativeTouchPosition);
        Vector3D projection = difference * projectionCoefficient;
        
        //NSLog(@"projection coefficient: %f", projectionCoefficient);
        
        Vector3D orthogonalComponent = relativeTouchPosition - projection;

        //NSLog(@"Orthogonal Component Magnitude: %f", orthogonalComponent.magnitude());
        
        if ( projectionCoefficient > 0 && projectionCoefficient < difference_mag &&
            orthogonalComponent.magnitude() < 100) {
            [touches setObject:touch forKey:@"pull_touch"];

            Vector3D velocity;
            GOBall* ball = [[[GOBall alloc] initWithPosition:touchPosition andVelocity:velocity] autorelease];
            [balls addObject:ball];
            
            stringMultiplier = 0;
        }
    }
}

- (void) destroySlingshot
{
    if ([touches objectForKey:@"pull_touch"] != nil) {
        if ([balls count])
            [balls removeLastObject];
    }
}

- (BOOL) slingshotPresent
{
    return [touches objectForKey:@"line_end_one"] != nil && [touches objectForKey:@"line_end_two"] != nil;
}

- (void) removeTouch:(UITouch*) touch
{
    if ([touches objectForKey:@"line_end_one"] == touch) {
        [touches removeObjectForKey:@"line_end_one"];
        [self destroySlingshot];
    }
    if ([touches objectForKey:@"line_end_two"] == touch) {
        [touches removeObjectForKey:@"line_end_two"];
        [self destroySlingshot];
    }
    if ([touches objectForKey:@"pull_touch"] == touch) {
        [touches removeObjectForKey:@"pull_touch"];
        if ([balls count]) {
            Vector3D velocity = [self getShotVelocity:touch];
            *((GOBall*)[balls lastObject]).velocity = velocity;
            stringMultiplier = velocity.magnitude() / 50;
        }
    }
}

- (void) moveTouch:(UITouch*) touch
{
    if ([touches objectForKey:@"line_end_one"] != touch &&
        [touches objectForKey:@"line_end_two"] != touch &&
        [touches objectForKey:@"pull_touch"] != touch) {

        UITouch* touch1 = [touches objectForKey:@"line_end_one"];
        CGPoint loc1 = [touch1 locationInView:touch1.view];
        Vector3D v1(loc1.x, loc1.y, 0);
        UITouch* touch2 = [touches objectForKey:@"line_end_two"];
        CGPoint loc2 = [touch2 locationInView:touch2.view];
        Vector3D v2(loc2.x, loc2.y, 0);
        
        Vector3D difference = v2 - v1;
        float difference_mag = difference.magnitude();
        difference.normalize();
        
        CGPoint touchPositionPoint = [touch locationInView:touch.view];
        Vector3D touchPosition(touchPositionPoint.x, touchPositionPoint.y, 0);
        Vector3D relativeTouchPosition(touchPosition.x - loc1.x, touchPosition.y - loc1.y, 0);
        
        float projectionCoefficient = (difference * relativeTouchPosition);
        Vector3D projection = difference * projectionCoefficient;
        
        //NSLog(@"projection coefficient: %f", projectionCoefficient);
        
        Vector3D orthogonalComponent = relativeTouchPosition - projection;
        
        //NSLog(@"Orthogonal Component Magnitude: %f", orthogonalComponent.magnitude());
        
        //NSLog(@"HEEY proj coef: %f, ortho mag: %f, diff mag: %f", projectionCoefficient, orthogonalComponent.magnitude(), difference_mag);
        
        if (projectionCoefficient > 0 && projectionCoefficient < difference_mag && orthogonalComponent.magnitude() < 100) {
            if (![touchesPluckable containsObject:touch]) {
                self.stringFrequency = difference_mag / 5;
                [touchesPluckable addObject:touch];
                stringMultiplier = 1.0;
            }
        } 
        else {
            [touchesPluckable removeObject:touch];
        }
    }
}

- (Vector3D) getShotVelocity:(UITouch*) touch
{
    if ([touches count] >= 2) {
        UITouch *t1 = [touches objectForKey:@"line_end_one"];
        CGPoint p1 = [t1 locationInView:t1.view];
        Vector3D v1; v1.x = p1.x; v1.y = p1.y;
        UITouch *t2 = [touches objectForKey:@"line_end_two"];
        CGPoint p2 = [t2 locationInView:t2.view];
        Vector3D v2; v2.x = p2.x; v2.y = p2.y;
        
        Vector3D center = v2 + (v1 - v2) * 0.5;
        
        CGPoint touch_loc = [touch locationInView:touch.view];
        Vector3D touch; touch.x = touch_loc.x; touch.y = touch_loc.y;
        
        Vector3D velocity = (center - touch) * 0.1;
        
        return velocity;
    }
    else {
        Vector3D zero;
        return zero;
    }
}

- (void) tick
{
    float tickSin = sin(0.5 * tickCount);
    for (int i = 0; i < num_line_points; i++) {
        [line_offsets insertObject:[NSNumber numberWithFloat:stringMultiplier * 100 * tickSin * sin(M_PI * i / num_line_points)] atIndex:i];
    }
    stringMultiplier *= 0.92;
    
    int num_to_delete = 0;
    for (GOBall* ball in balls) {
        if ([self ageAndDeleteBall:ball]) {
            num_to_delete++;
        }
        
        ball.position->x += ball.velocity->x;
        ball.position->y += ball.velocity->y;
        
        if (self.gravity) {
            ball.velocity->y += 0.7;
        }
        
        *ball.velocity *= (1 - self.damping);
    }
    
    for (GOBall* ball in balls) {
        [self bounceBallOffScreenEdges:ball];
        [self bounceBallOffBalls:ball];
    }
    
    while(num_to_delete > 0) {
        [self.balls removeObjectAtIndex:0];
        num_to_delete--;
    }
    
    for (GOCollision* collision in collisions) {
        if ([self ageAndDeleteCollision:collision]) {
            num_to_delete++;
        }
    }
    
    while(num_to_delete > 0) {
        [self.collisions removeObjectAtIndex:0];
        num_to_delete--;
    }
    
    if ([self slingshotPresent]) {
        UITouch *t1 = [touches objectForKey:@"line_end_one"];
        CGPoint p1 = [t1 locationInView:t1.view];
        Vector3D v1; v1.x = p1.x; v1.y = p1.y;
        UITouch *t2 = [touches objectForKey:@"line_end_two"];
        CGPoint p2 = [t2 locationInView:t2.view];
        Vector3D v2; v2.x = p2.x; v2.y = p2.y;
        
        Vector3D center = v2 + (v1 - v2) * 0.5;
        
        UITouch* pull_touch = [touches objectForKey:@"pull_touch"];
        if (pull_touch != nil) {
            CGPoint pull_point = [pull_touch locationInView:pull_touch.view];
            center.x = pull_point.x; center.y = pull_point.y;
            
            // special case: update position of soon-to-release ball
            ((GOBall*)[balls lastObject]).position->x = pull_point.x;
            ((GOBall*)[balls lastObject]).position->y = pull_point.y;
            // continued hacky special case: never age the held ball
            ((GOBall*)[balls lastObject]).age = 0;
        }
        
        int half_points = ceil(num_line_points / 2.0);
        
        Vector3D difference = v1 - center;
        
        for (int i = 0; i < half_points; i++) {
            NSValue* pointValue = [line_points objectAtIndex:i];
            Vector3D* point = (Vector3D*)[pointValue pointerValue];
            Vector3D segment = v1 - difference * (((float)i + 1) / half_points);
            *point = segment;
        }

        difference = center - v2;
        
        for (int i = half_points; i < num_line_points; i++) {
            NSValue* pointValue = [line_points objectAtIndex:i];
            Vector3D* point = (Vector3D*)[pointValue pointerValue];
            Vector3D segment = center - difference * ((float)(i - half_points + 1) / half_points);
            *point = segment;
        }
    }
    
    tickCount++;
}

- (BOOL) ageAndDeleteBall:(GOBall*)ball
{
    ball.age++;
    if (ball.age >= ball.lifetime) {
        return YES;
    }
    return NO;
}

- (BOOL) ageAndDeleteCollision:(GOCollision*)collision
{
    collision.age++;
    if (collision.age >= collision.lifetime) {
        return YES;
    }
    return NO;
}

- (void) bounceBallOffScreenEdges:(GOBall*) ball
{
    if (ball.position->x - ball.radius < 0) {
        ball.velocity->x *= -0.9;
        ball.position->x = ball.radius;
        self.num_wall_impacts++;
    }
    else if (ball.position->x + ball.radius > 768) {
        ball.velocity->x *= -0.9;
        ball.position->x = 768 - ball.radius;
        self.num_wall_impacts++;
    }

    if (ball.position->y - ball.radius < 0) {
        ball.velocity->y *= -0.9;
        ball.position->y = ball.radius;
        self.num_wall_impacts++;
    }
    else if (ball.position->y + ball.radius > 1024) {
        ball.velocity->y *= -0.9;
        ball.position->y = 1024 - ball.radius;
        self.num_wall_impacts++;
    }
    
}

- (void) bounceBallOffBalls:(GOBall*) ball
{
    GLfloat distance;
    Vector3D ballPos; ballPos.x = ball.position->x; ballPos.y = ball.position->y;
    Vector3D otherBallPos, difference, otherDifference;

    for (GOBall* otherBall in self.balls) {
        if (ball == otherBall) {
            continue;
        }
        
        otherBallPos.x = otherBall.position->x; otherBallPos.y = otherBall.position->y;

        difference = otherBallPos - ballPos;
        distance = difference.magnitude();
        difference.normalize();
                
        if (distance < ball.radius + otherBall.radius) {
        
            Vector3D projectedBallVelocity = difference * (*ball.velocity * difference);
            Vector3D projectedOtherBallVelocity = (difference * -1) * (*otherBall.velocity * (difference * -1));
            
            *ball.velocity -= projectedBallVelocity;
            *otherBall.velocity -= projectedOtherBallVelocity;

            *ball.velocity += projectedOtherBallVelocity;
            *otherBall.velocity += projectedBallVelocity;
            
            *otherBall.position = *ball.position + difference * (ball.radius + otherBall.radius + 1);
            
            [collisions addObject:[[GOCollision alloc] initWithPosition:*ball.position + difference * ball.radius]];
            num_ball_impacts++;
        }
    }
}

- (void) dealloc
{
    [touches release];
    [balls release];
    [collisions release];
    [super dealloc];
}

@end


@implementation GOBall

@synthesize position;
@synthesize velocity;
@synthesize age;
@synthesize lifetime;
@synthesize radius;

- (id) initWithPosition:(Vector3D)aPosition andVelocity:(Vector3D)aVelocity
{
    self = [super init];
    if (self) {
        self.position = new Vector3D(aPosition);
        self.velocity = new Vector3D(aVelocity);
        
        self.age = 0;
        self.lifetime = BALL_LIFETIME;
        
        self.radius = 50 + ((rand() % 20) - 10);        
    }
    return self;
}

- (void) dealloc
{
    free(self.position);
    free(self.velocity);
    
    [super dealloc];
}

@end

@implementation GOCollision

@synthesize position;
@synthesize age;
@synthesize lifetime;

- (id)initWithPosition:(Vector3D)aPosition
{
    self = [super init];
    if (self) {
        self.position = new Vector3D(aPosition);
        self.age = 0;
        self.lifetime = COLLISION_LIFETIME;
    }
    return self;
}

- (void) dealloc
{
    free(self.position);
    
    [super dealloc];
}

@end

