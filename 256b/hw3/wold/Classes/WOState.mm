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
#import "ASIFormDataRequest.h"

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
        
        NSArray* planetPlist = [self getPlanetsFromServer];
        NSDictionary* lastPlanet = [planetPlist lastObject];
        
        self.planets = [[[NSMutableSet alloc] init] autorelease];
        Vector3D planetPosition(-2, 0, -6); // TODO: replace this with per-planet position from db
        //Vector3D planetPosition(0, 0, -6);
        WOPlanet* planet = [[WOPlanet alloc] initWithID: [[lastPlanet objectForKey:@"id"] intValue]
                                            andPosition: planetPosition 
                                              andRadius: 2
                                             andTexture: g_texture[0] // TODO: replace with handle to texture from db
                                         andTreeTexture: g_texture[1] // TODO: probably get rid of
                                                 andRed: [[lastPlanet objectForKey:@"red"] floatValue]
                                               andGreen: [[lastPlanet objectForKey:@"green"] floatValue]
                                                andBlue: [[lastPlanet objectForKey:@"blue"] floatValue]];
        [self.planets addObject:planet];
    }
    return self;
}

- (NSArray*) getPlanetsFromServer
{
    NSArray* planetsPlist;
    
    NSString *server = [[NSString alloc] initWithString:@"http://mikerotondo.com/wold/"];
    
    NSURL *url = [NSURL URLWithString:[server stringByAppendingString:@"planets"]];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    
    // Start sending synchronously
    [request startSynchronous];
    
    // Display any error messages
    NSError *error = [request error];
    if (!error) {
        NSString *response = [request responseString];
        NSLog(@"Response: %@", response);
        
        // deserialize the downloaded data from plist to NSArrays, NSDictionaries, NSStrings, and the like.
        NSData* data = [NSData dataWithBytes:[response cStringUsingEncoding:NSUTF8StringEncoding] 
                                      length:[response lengthOfBytesUsingEncoding:NSUTF8StringEncoding]];
        planetsPlist = [NSPropertyListSerialization propertyListWithData:data
                                                                 options:0
                                                                  format:NULL
                                                                   error:NULL];
        
        // make sure we got an array
        if ([planetsPlist isKindOfClass:[NSArray class]])
        {
            NSLog(@"got planets");
            
            // combine the messages into one long string to put in the text box
            NSMutableString *allPlanets = [NSMutableString string];
            
            for (NSDictionary *planetDict in planetsPlist)
            {
                [allPlanets appendFormat:@"%@, %@, %@ - %@\n",
                 [planetDict objectForKey:@"red"],
                 [planetDict objectForKey:@"green"],
                 [planetDict objectForKey:@"blue"],
                 [planetDict objectForKey:@"texture"]];
            }
            
            // put the text in the text box
            NSLog(@"All planets: %@", allPlanets);
        }
        else
        {
            NSLog(@"couldn't parse planets: %@", planetsPlist);
        }
        
        
    } else {
        NSLog(@"An error! %@", error);
    }
    return planetsPlist;
}

- (void) render
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    for (WOPlanet* planet in self.planets) {
        [planet tick];
        [planet render];
    }
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
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
    [currentPlanet stopGrowingAndSendToServer:YES];
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
