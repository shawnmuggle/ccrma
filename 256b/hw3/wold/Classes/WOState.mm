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

@synthesize planets;

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
        
        self.planets = [[[NSMutableSet alloc] init] autorelease];
        Vector3D planetPosition(-2, 0, -6);
        WOPlanet* planet = [[WOPlanet alloc] initWithPosition:planetPosition andRadius:2 andTexture:g_texture[0] andTreeTexture:g_texture[1]]; // 3
        [self.planets addObject:planet];
        
    }
    return self;
}

- (void) render
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    for (WOPlanet* planet in self.planets) {
        [planet tick];
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

- (void) processPressEnd
{
    WOPlanet* currentPlanet = [self.planets anyObject];
    [currentPlanet stopGrowing];
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
    [self.planets release];
    
    [super dealloc];
}

@end
