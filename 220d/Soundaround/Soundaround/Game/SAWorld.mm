//
//  SAWorld.m
//  Soundaround
//
//  Created by Michael Rotondo on 10/20/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//


#import "SAWorld.h"
#import "SABall.h"
#include <Box2D/Box2D.h>

@interface SAWorld ()

@property (nonatomic, assign) float physicsHz;
@property (nonatomic, retain) NSTimer *physicsTimer;

@end

@implementation SAWorld
@synthesize terrainPoints;
@synthesize physicsWorld;
@synthesize ball;
@synthesize physicsTimer;
@synthesize physicsHz;

+ (id)world
{
    srand((unsigned int)time(NULL));
    
    SAWorld *world = [[[SAWorld alloc] init] autorelease];

    b2Vec2 gravity(0.0f, -10.0f);
    world.physicsWorld = new b2World(gravity);
    
    b2BodyDef bd;
    b2Body* ground = world.physicsWorld->CreateBody(&bd);
    
    int numTerrainPoints = 100;
    float terrainSegmentWidth = 10; // in meters
    world.terrainPoints = [NSMutableArray arrayWithCapacity:numTerrainPoints];
    
    float prevX = 0.0f;
    float prevY = 0.0f;
    float smoothing = 0.9f;
    for (int i = 0; i < numTerrainPoints; i++)
    {
        float x = terrainSegmentWidth * i;
        float y = smoothing * prevY + (1 - smoothing) * 10 * (rand() / (float)RAND_MAX);
        [world.terrainPoints addObject:[NSValue valueWithPoint:CGPointMake(x, y)]];
        
        b2EdgeShape shape;
        shape.Set(b2Vec2(prevX, prevY), b2Vec2(x, y));
        ground->CreateFixture(&shape, 0.0f);

        prevX = x;
        prevY = y;
    }

    world.ball = [SABall ballInWorld:world];
    
    world.physicsHz = 1.0f/60.0f;
    world.physicsTimer = [NSTimer timerWithTimeInterval:world.physicsHz target:world selector:@selector(update:) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:world.physicsTimer forMode:NSDefaultRunLoopMode];
    
    return world;
}

- (void)dealloc
{
    [terrainPoints release];
    [physicsTimer release];
    delete physicsWorld;
    [super dealloc];
}

- (void)update:(NSTimer *)timer
{
    int velocityIterations = 8;
    int positionIterations = 3;
    self.physicsWorld->Step(self.physicsHz, velocityIterations, positionIterations);
}

- (void)draw
{    
    //    glClearColor(0, 0, 0, 0);
    //    glClear(GL_COLOR_BUFFER_BIT);
    
    glClearColor(
                 sin(CFAbsoluteTimeGetCurrent()),
                 sin(7.0*CFAbsoluteTimeGetCurrent()),
                 sin(CFAbsoluteTimeGetCurrent()/3.0),0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(10.0f);
    
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (NSValue *pointValue in self.terrainPoints)
        {
            CGPoint point = [pointValue pointValue];
            glVertex2d(point.x, point.y);
            glVertex2d(point.x, 0);
        }
    }
    glEnd();
    
    [self.ball draw];
}

- (void)setTargetPoint:(CGPoint)targetPoint
{
    [self.ball throwTowards:targetPoint];
}

@end
