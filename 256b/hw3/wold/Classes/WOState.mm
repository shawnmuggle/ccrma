//
//  WOState.m
//  wold
//
//  Created by Michael Rotondo on 2/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOState.h"
#import "ES1Renderer.h"
#import "WOLSystem.h"
#import "WOPlanet.h"

GLuint g_texture[2];

@implementation WOState

@synthesize growingTrees;
@synthesize trees;
@synthesize planets;
@synthesize camera;

- (id) init
{
    self = [super init];
    if (self) {
        glGenTextures( 2, &g_texture[0] );
        // bind the texture
        glBindTexture( GL_TEXTURE_2D, g_texture[0] );
        // setting parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        // load the texture
        MoGfx::loadTexture( @"venusbump", @"jpg" );
        
        // bind the texture
        glBindTexture( GL_TEXTURE_2D, g_texture[1] );
        // setting parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        // load the texture
        MoGfx::loadTexture( @"white_texture", @"png" );
        
        self.growingTrees = [[[AKTouchTracker alloc] init] autorelease];
        self.trees = [[[NSMutableSet alloc] init] autorelease];
        
        self.planets = [[[NSMutableSet alloc] init] autorelease];
        //Vector3D planetPosition(1, 1.5, -6);
        Vector3D planetPosition(-2, 0, -6);
        WOPlanet* planet = [[WOPlanet alloc] initWithPosition:planetPosition andRadius:2 andTexture:g_texture[0] andTreeTexture:g_texture[1]]; // 3
        [self.planets addObject:planet];
        
        self.camera = [[[WOCamera alloc] init] autorelease];
        self.camera.planet = planet;
    }
    return self;
}

- (void) addTouch:(UITouch*) touch
{
//    WOLSystem* tree = [[WOLSystem alloc] initWithMaxGeneration:4 atPoint:[touch locationInView:touch.view]];
//
//    WOXNode* xNode = [[WOXNode alloc] init];
//    [tree.nodes addObject:xNode];
//
//    [self.growingTrees setObject:tree forTouch:touch];
//    [self.trees addObject:tree];
}

- (void) removeTouch:(UITouch*) touch
{
//    [self.growingTrees removeObjectForTouch:touch];
}

- (void) moveTouch:(UITouch*) touch
{
//    CGPoint touchLoc = [touch locationInView:touch.view];
//    WOLSystem* tree = [self.growingTrees objectForTouch:touch];
//    [tree setOffset:CGPointMake(touchLoc.x - tree.origin.x, tree.origin.y - touchLoc.y)];
}

- (void) tick
{
    for (UITouch* touch in [self.growingTrees allTouches]) {
        WOLSystem* tree = [self.growingTrees objectForTouch:touch];
        [tree tick];
    }
}

- (void) transformModelviewMatrix
{
    [self.camera transform];    
}

- (void) render
{
//    for (WOLSystem* tree in self.trees) {
//        [tree render];
//    }
    
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    for (WOPlanet* planet in self.planets) {
        [planet render];
    }
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

- (void) handleTouchRay:(Vector3D)ray fromPoint:(Vector3D)touchPoint
{
    BOOL hit = NO;
    
    //NSLog(@"Stepping into space...");
    int numSteps = 100;
    for (int i = 0; i < numSteps; i++) {
        Vector3D point = touchPoint + ray * ((float)i / numSteps);
        //NSLog(@"Point @ %f, %f, %f", point.x, point.y, point.z);
        Vector3D dist;
        for (WOPlanet* planet in self.planets) {
            //NSLog(@"Planet @ %f, %f, %f", planet.position.x, planet.position.y, planet.position.z);
            dist = point - planet.position;
            if (dist.magnitude() < planet.radius) {
                //NSLog(@"O MF G A HIT!");
                //NSLog(@"Dist: %f", dist.magnitude());
                [planet addPoint:point];
                hit = YES;
            }
        }
        if (hit) {
            break;
        }
    }
}

- (void) handleTapRay:(Vector3D)ray fromPoint:(Vector3D)touchPoint
{
    BOOL hit = NO;
    
    //NSLog(@"Stepping into space...");
    int numSteps = 100;
    for (int i = 0; i < numSteps; i++) {
        Vector3D point = touchPoint + ray * ((float)i / numSteps);
        //NSLog(@"Point @ %f, %f, %f", point.x, point.y, point.z);
        Vector3D dist;
        for (WOPlanet* planet in self.planets) {
            //NSLog(@"Planet @ %f, %f, %f", planet.position.x, planet.position.y, planet.position.z);
            dist = point - planet.position;
            if (dist.magnitude() < planet.radius) {
                //NSLog(@"O MF G A HIT!");
                //NSLog(@"Dist: %f", dist.magnitude());
                [planet processTap:point];
                hit = YES;
            }
        }
        if (hit) {
            break;
        }
    }
}

- (void)processDrag:(UIPanGestureRecognizer*)gesture
{
    WOPlanet* currentPlanet = [self.planets anyObject];
    [currentPlanet processDrag:gesture];
}

- (void) dealloc
{
    [self.growingTrees release];
    [self.trees release];
    [self.planets release];
    [self.camera release];
    
    [super dealloc];
}

@end
