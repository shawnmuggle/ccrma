//
//  SAWorld.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/20/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>
#include "OpenGL/gl.h"
#import "SAWorld.h"
#import "SABall.h"
#import "SAAvatar.h"
#import "SAGrassBlade.h"
#import "SASoundPath.h"
#import "SABallGuide.h"
#import "SABallGuidePoint.h"

#define NUM_TERRAIN_POINTS_PER_HUNK 100

// Begin private stuff

class SAPhysicsContactListener : public b2ContactListener
{
public:
    SAPhysicsContactListener(NSDictionary *aBodyObjectsMap) { bodyObjects = aBodyObjectsMap; }
    void BeginContact(b2Contact* contact) {} // Don't care
    void EndContact(b2Contact* contact) {}  // Don't care
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {}  // Don't care
    
private:
    NSDictionary *bodyObjects;
};

@interface SAWorld ()
{
    NSMutableArray *terrainPoints;
    float physicsHz;
    NSTimer *physicsTimer;
    SAPhysicsContactListener *physicsContactListener;
    NSMutableDictionary *bodyObjects;
    NSMutableArray *terrainDoodads;
    NSMutableArray *soundPaths;
    NSMutableSet *ballGuides;
    SABallGuide *currentBallGuide;
}

- (void)createTerrain;

@end

// End private stuff

void SAPhysicsContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());

    const b2Body* bodyA = contact->GetFixtureA()->GetBody();
    const b2Body* bodyB = contact->GetFixtureB()->GetBody();
    SAPhysicsObject *objectA = [bodyObjects objectForKey:[NSValue valueWithPointer:bodyA]];
    SAPhysicsObject *objectB = [bodyObjects objectForKey:[NSValue valueWithPointer:bodyB]];

    if ([objectA shouldDisableContact:contact] || [objectB shouldDisableContact:contact])
    {
        contact->SetEnabled(false);
    }
    else
    {
        if (state2[0] == b2_addState)  // This is a new contact
        {
            [objectA processContact:contact withOtherObject:objectB];
            [objectB processContact:contact withOtherObject:objectA];
        }
    }
}

@implementation SAWorld
@synthesize ball;
@synthesize avatar;
@synthesize physicsWorld;
@synthesize editMode;

- (id)init
{
    self = [super init];
    if (self)
    {
        srand((unsigned int)time(NULL));
        
        bodyObjects = [NSMutableDictionary dictionaryWithCapacity:3];
        terrainDoodads = [NSMutableArray arrayWithCapacity:100];
        
        b2Vec2 gravity(0.0f, -10.0f);
        self.physicsWorld = new b2World(gravity);
        
        [self createTerrain];
        [self registerBody:self.physicsBody withObject:self];
        
        self.ball = [[SABall alloc] initWithWorld:self];
        self.avatar = [[SAAvatar alloc] initWithWorld:self andBall:self.ball];
        
        physicsHz = 1.0f/60.0f;
        physicsTimer = [NSTimer timerWithTimeInterval:physicsHz target:self selector:@selector(update:) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:physicsTimer forMode:NSDefaultRunLoopMode];
        
        physicsContactListener = new SAPhysicsContactListener(bodyObjects);
        self.physicsWorld->SetContactListener(physicsContactListener);
        
        soundPaths = [NSMutableArray arrayWithCapacity:100];
        ballGuides = [NSMutableSet setWithCapacity:100];

        self.editMode = NO;
    }
    return self;
}

- (void)createTerrain
{
    b2BodyDef bd;
    self.physicsBody = self.physicsWorld->CreateBody(&bd);
    
    float terrainSegmentWidth = 20; // in meters
    terrainPoints = [NSMutableArray arrayWithCapacity:NUM_TERRAIN_POINTS_PER_HUNK];
    
    b2Vec2 vertices[NUM_TERRAIN_POINTS_PER_HUNK];
    
    float prevY = 10.0f;
    float prevX = 0.0f;
    float smoothing = 0.2f;
    for (int i = 0; i < NUM_TERRAIN_POINTS_PER_HUNK; i++)
    {
        float x = terrainSegmentWidth * i;
        float y = smoothing * prevY + (1 - smoothing) * 10 * (rand() / (float)RAND_MAX);
        [terrainPoints addObject:[NSValue valueWithPoint:CGPointMake(x, y)]];
        
        vertices[i].Set(x, y);
        
        // CREATE SOmE GRASS?!?
//        SAGrassBlade *grassblade = [SAGrassBlade grassbladeWithWorld:self atPosition:CGPointMake(x, y)];
//        [self.terrainDoodads addObject:grassblade];

        prevY = y;
        prevX = x;
    }

    b2ChainShape terrainChain;
    terrainChain.CreateChain(vertices, NUM_TERRAIN_POINTS_PER_HUNK);
    
    b2FixtureDef fd;
    
    fd.shape = &terrainChain;
    fd.friction = 0.1f;
    self.physicsBody->CreateFixture(&fd);

//    self.physicsBody->CreateFixture(&terrainChain, 0.0f);
}

- (void)dealloc
{
    delete physicsWorld;
}

- (void)registerBody:(b2Body *)body withObject:(id)object
{
    [bodyObjects setObject:object forKey:[NSValue valueWithPointer:body]];
}

- (void)update:(NSTimer *)timer
{
    if ( !self.editMode)
    {
        [self.ball update];
        [self.avatar update];
        
        int velocityIterations = 8;
        int positionIterations = 3;
        self.physicsWorld->Step(physicsHz, velocityIterations, positionIterations);
        
        if (!ball.currentGuide && !ball.held)
        {
            for (SABallGuide *ballGuide in ballGuides)
            {
                if ([ballGuide shouldActivateInWorld:self])
                {
                    [ballGuide activateWithBall:self.ball];
                    break;
                }
            }
        }
        for (SABallGuide *ballGuide in ballGuides)
        {
            [ballGuide updateInWorld:self withTimeInterval:timer.timeInterval];
        }
    }
}

- (void)draw
{    
    //    glClearColor(0, 0, 0, 0);
    //    glClear(GL_COLOR_BUFFER_BIT);
    
    glClearColor(0.2826,
                 0.5174,
                 0.9957, 
                 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(10.0f);
    
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (NSValue *pointValue in terrainPoints)
        {
            glColor3f(0.2043, 
                      0.5000, 
                      0.1870);
            CGPoint point = [pointValue pointValue];
            glVertex2d(point.x, point.y);
            glVertex2d(point.x, 0);
        }
    }
    glEnd();
    
    [self.ball draw];
    [self.avatar draw];
    for (id<SADrawableObject> doodad in terrainDoodads)
    {
        [doodad draw];
    }
    for (SASoundPath *soundPath in soundPaths)
    {
        [soundPath draw];
    }
    
    for (SABallGuide *ballGuide in ballGuides)
    {
        [ballGuide draw];
    }
    if (currentBallGuide)
        [currentBallGuide draw];

}

- (void)processContact:(b2Contact *)contact withOtherObject:(SAPhysicsObject *)otherObject;
{
//    NSLog(@"TERRAIN COLLISION");
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    
    [self.ball audioProducer:self.ball fillBuffer:output withNumFrames:numBufferFrames];
    [self.avatar audioProducer:self.avatar fillBuffer:output withNumFrames:numBufferFrames];
    for (SABallGuide *ballGuide in ballGuides)
    {
        [ballGuide audioProducer:ballGuide fillBuffer:output withNumFrames:numBufferFrames];
    }
}

- (void)addSoundPath:(SASoundPath *)inSoundPath
{
    [soundPaths addObject:inSoundPath];
}

- (void)toggleEditMode
{
    self.editMode = !self.editMode;
}

- (void)startBallGuide
{
    currentBallGuide = [[SABallGuide alloc] init];
}

- (void)addBallGuidePoint:(CGPoint)point
{
    [currentBallGuide addPoint:[[SABallGuidePoint alloc] initWithPosition:point]];
}

- (void)finishBallGuide
{
    if (currentBallGuide && [currentBallGuide isValid])
    {
        [ballGuides addObject:currentBallGuide];
    }
    currentBallGuide = nil;
}

@end
