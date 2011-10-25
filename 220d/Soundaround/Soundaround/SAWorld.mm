//
//  SAWorld.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/20/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>

#import "SAWorld.h"
#import "SABall.h"
#import "SAAvatar.h"
#import "SAGrassBlade.h"

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

@property (nonatomic, retain) NSMutableArray *terrainPoints;
@property (nonatomic, assign) float physicsHz;
@property (nonatomic, retain) NSTimer *physicsTimer;
@property (nonatomic, assign) SAPhysicsContactListener *physicsContactListener;
@property (nonatomic, retain) NSMutableDictionary *bodyObjects;
@property (nonatomic, retain) NSMutableArray *terrainDoodads;


- (void)createTerrain;

@end

// End private stuff

void SAPhysicsContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    if (state2[0] == b2_addState)  // This is a new contact
    {
        const b2Body* bodyA = contact->GetFixtureA()->GetBody();
        const b2Body* bodyB = contact->GetFixtureB()->GetBody();
        // Use a SAPhysicsBody superclass to avoid ids here
        id objectA = [bodyObjects objectForKey:[NSValue valueWithPointer:bodyA]];
        id objectB = [bodyObjects objectForKey:[NSValue valueWithPointer:bodyB]];
        [objectA processContact:contact withOtherObject:objectB];
        [objectB processContact:contact withOtherObject:objectA];
    }
}

@implementation SAWorld
@synthesize ball;
@synthesize avatar;
@synthesize terrainPoints;
@synthesize physicsWorld;
@synthesize physicsHz;
@synthesize physicsTimer;
@synthesize physicsContactListener;
@synthesize bodyObjects;
@synthesize terrainDoodads;

+ (id)world
{
    srand((unsigned int)time(NULL));
    
    SAWorld *world = [[[SAWorld alloc] init] autorelease];
    world.bodyObjects = [NSMutableDictionary dictionaryWithCapacity:3];
    world.terrainDoodads = [NSMutableArray arrayWithCapacity:100];

    b2Vec2 gravity(0.0f, -10.0f);
    world.physicsWorld = new b2World(gravity);
    
    [world createTerrain];
    [world registerBody:world.physicsBody withObject:world];
    
    world.ball = [SABall ballInWorld:world];
    world.avatar = [SAAvatar avatarInWorld:world];
    
    world.physicsHz = 1.0f/60.0f;
    world.physicsTimer = [NSTimer timerWithTimeInterval:world.physicsHz target:world selector:@selector(update:) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:world.physicsTimer forMode:NSDefaultRunLoopMode];
    
    world.physicsContactListener = new SAPhysicsContactListener(world.bodyObjects);
    world.physicsWorld->SetContactListener(world.physicsContactListener);
    
    return world;
}

- (void)createTerrain
{
    b2BodyDef bd;
    self.physicsBody = self.physicsWorld->CreateBody(&bd);
    
    float terrainSegmentWidth = 10; // in meters
    self.terrainPoints = [NSMutableArray arrayWithCapacity:NUM_TERRAIN_POINTS_PER_HUNK];
    
    b2Vec2 vertices[NUM_TERRAIN_POINTS_PER_HUNK];
    
    float prevY = 0.0f;
    float prevX = 0.0f;
    float smoothing = 0.5f;
    for (int i = 0; i < NUM_TERRAIN_POINTS_PER_HUNK; i++)
    {
        float x = terrainSegmentWidth * i;
        float y = smoothing * prevY + (1 - smoothing) * 10 * (rand() / (float)RAND_MAX);
        [self.terrainPoints addObject:[NSValue valueWithPoint:CGPointMake(x, y)]];
        
        vertices[i].Set(x, y);
        
        // CREATE SOmE GRASS?!?
//        SAGrassBlade *grassblade = [SAGrassBlade grassbladeWithWorld:self atPosition:CGPointMake(x, y)];
//        [self.terrainDoodads addObject:grassblade];

        prevY = y;
        prevX = x;
    }

    b2ChainShape terrainChain;
    terrainChain.CreateChain(vertices, NUM_TERRAIN_POINTS_PER_HUNK);
    self.physicsBody->CreateFixture(&terrainChain, 0.0f);
}

- (void)dealloc
{
    [terrainPoints release];
    [physicsTimer release];
    [bodyObjects release];
    [terrainDoodads release];
    delete physicsWorld;
    [super dealloc];
}

- (void)registerBody:(b2Body *)body withObject:(id)object
{
    [self.bodyObjects setObject:object forKey:[NSValue valueWithPointer:body]];
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
    
    glClearColor(0.2826,
                 0.5174,
                 0.9957, 
                 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(10.0f);
    
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (NSValue *pointValue in self.terrainPoints)
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
    for (id<SADrawableObject> doodad in self.terrainDoodads)
    {
        [doodad draw];
    }
}

- (void)processContact:(b2Contact *)contact withOtherObject:(SAPhysicsObject *)otherObject;
{
//    NSLog(@"TERRAIN COLLISION");
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    
    [self.ball audioProducer:self.ball fillBuffer:output withNumFrames:numBufferFrames];
    [self.avatar audioProducer:self.avatar fillBuffer:output withNumFrames:numBufferFrames];
}

#pragma mark Control

- (void)setTargetPoint:(CGPoint)targetPoint
{
    [self.ball throwTowards:targetPoint];
}

@end
