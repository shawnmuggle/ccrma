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
#import "SAUtils.h"

#define NUM_TERRAIN_POINTS_PER_HUNK 100
#define NUM_CURVE_POINTS_PER_SEGMENT 20

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
    b2ChainShape *terrainChain;
}

- (void)createTerrain;
- (CGPoint)tangentAtIndex:(int)index inPoints:(NSArray *)points;

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

- (CGPoint)tangentAtIndex:(int)i inPoints:(NSArray *)inPoints
{
    if ( i < 0 || i >= [inPoints count])
    {
        [NSException raise:@"Invalid index" format:@"index of %d is invalid", i];
    }
    
    NSValue *prevPointValue = [inPoints objectAtIndex:MAX(i - 1, 0)];
    NSValue *pointValue = [inPoints objectAtIndex:i];
    NSValue *nextPointValue = [inPoints objectAtIndex:MIN(i + 1, [inPoints count] - 1)];

    CGPoint prevPoint = [prevPointValue pointValue];
    CGPoint point = [pointValue pointValue];
    CGPoint nextPoint = [nextPointValue pointValue];
    
    if (i == 0)
    {
        return CGPointSubtract(nextPoint, point);
    }
    if (i == [inPoints count] - 1)
    {
        return CGPointSubtract(point, prevPoint);
    }
    else
    {
        return CGPointDivide(CGPointSubtract(nextPoint, prevPoint), 2.0f);
    }
}

- (void)createTerrain
{
    b2BodyDef bd;
    self.physicsBody = self.physicsWorld->CreateBody(&bd);
    
    float terrainSegmentWidth = 20; // in meters
    NSMutableArray *uncurvedTerrainPoints = [NSMutableArray arrayWithCapacity:NUM_TERRAIN_POINTS_PER_HUNK];
    
    float prevY = 10.0f;
    float prevX = 0.0f;
    float smoothing = 0.2f;
    for (int i = 0; i < NUM_TERRAIN_POINTS_PER_HUNK; i++)
    {
        float x = terrainSegmentWidth * i;
        float y = smoothing * prevY + (1 - smoothing) * 10 * (rand() / (float)RAND_MAX);
        [uncurvedTerrainPoints addObject:[NSValue valueWithPoint:CGPointMake(x, y)]];
        
        prevY = y;
        prevX = x;
    }

    // We only do bezier interpolation up to the last major point, not after it.
    terrainPoints = [NSMutableArray arrayWithCapacity:(NUM_TERRAIN_POINTS_PER_HUNK - 1) * NUM_CURVE_POINTS_PER_SEGMENT + 1];
    b2Vec2 vertices[(NUM_TERRAIN_POINTS_PER_HUNK - 1) * NUM_CURVE_POINTS_PER_SEGMENT + 1];

    for (int i = 0; i < NUM_TERRAIN_POINTS_PER_HUNK - 1; i++)
    {
        NSValue *pointValue = [uncurvedTerrainPoints objectAtIndex:i];
        NSValue *nextPointValue = [uncurvedTerrainPoints objectAtIndex:MIN(i + 1, [uncurvedTerrainPoints count])];

        CGPoint point = [pointValue pointValue];
        CGPoint nextPoint = [nextPointValue pointValue];
        
        CGPoint tangentA = [self tangentAtIndex:i inPoints:uncurvedTerrainPoints];
        CGPoint tangentB = [self tangentAtIndex:i+1 inPoints:uncurvedTerrainPoints];
        
        CGPoint a = point;
        CGPoint b = CGPointAdd(a, CGPointDivide(tangentA, 3.0f));
        CGPoint d = nextPoint;
        CGPoint c = CGPointSubtract(d, CGPointDivide(tangentB, 3.0f));
        
        for (int j = 0; j < NUM_CURVE_POINTS_PER_SEGMENT; j++)
        {
            float t = (float) j / NUM_CURVE_POINTS_PER_SEGMENT;
            CGPoint bezierPoint = CGPointBezier(a, b, c, d, t);
            [terrainPoints addObject:[NSValue valueWithPoint:bezierPoint]];
            vertices[i * NUM_CURVE_POINTS_PER_SEGMENT + j].Set(bezierPoint.x, bezierPoint.y);
        }
    }
    
    NSValue *lastPointValue = [uncurvedTerrainPoints lastObject];
    CGPoint lastPoint = [lastPointValue pointValue];
    vertices[(NUM_TERRAIN_POINTS_PER_HUNK - 1) * NUM_CURVE_POINTS_PER_SEGMENT].Set(lastPoint.x, lastPoint.y);
    [terrainPoints addObject:lastPointValue];
    
    terrainChain = new b2ChainShape();
    terrainChain->CreateChain(vertices, (NUM_TERRAIN_POINTS_PER_HUNK - 1) * NUM_CURVE_POINTS_PER_SEGMENT + 1);
    
    b2FixtureDef fd;
    
    fd.shape = terrainChain;
    fd.friction = 0.1f;
    self.physicsBody->CreateFixture(&fd);
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
    glClearColor(0.2826,
                 0.5174,
                 0.9957, 
                 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBegin(GL_TRIANGLE_STRIP);
    {
//        for (NSValue *pointValue in terrainPoints)
        for (int i = 0; i < terrainChain->m_count; i++)
        {
            b2Vec2 point = terrainChain->m_vertices[i];
            
            glColor3f(0.2043, 
                      0.5000, 
                      0.1870);
//            CGPoint point = [pointValue pointValue];
            glVertex2d(point.x, point.y);
            glVertex2d(point.x, 0);
        }
    }
    glEnd();

    glColor3f(0.75f, 0.65f, 0.8f);
    glLineWidth(4.0f);
    glBegin(GL_LINE_STRIP);
    {
//        for (NSValue *pointValue in terrainPoints)
//        {
//            CGPoint point = [pointValue pointValue];
        for (int i = 0; i < terrainChain->m_count; i++)
        {
            b2Vec2 point = terrainChain->m_vertices[i];
            glVertex2d(point.x, point.y);
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
