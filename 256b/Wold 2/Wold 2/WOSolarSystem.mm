//
//  WOSolarSystem.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOSolarSystem.h"
#import "ASIFormDataRequest.h"

@implementation WOSolarSystem
@synthesize star, planets, sky, zoom;

- (id) initWithId:(int)newSystemId;
{
    self = [super init];
    if (self) {
        systemId = newSystemId;
        
        self.sky = [CALayer layer];
        self.zoom = [CALayer layer];
        [self.sky addSublayer:self.zoom];
        
        CGPoint starLoc = CGPointMake(1000, 2000);
        //CGPoint starLoc = CGPointMake(0,0);
        self.star = [[[WOStar alloc] initAtPoint:starLoc] autorelease];
        [self.zoom addSublayer:self.star.translation];
        
        
        int numPlanets = 6;
        
        self.planets = [NSMutableArray arrayWithCapacity:numPlanets];
        float dist, angle;
        
        // TODO: randomize distances, angles
        // TODO: draw orbit lines
        
        bool createNewSystem = NO;
        NSArray* planetPlists;
        if (!createNewSystem) {
            planetPlists = [[self getPlanetsFromServer] retain];
        }
        
        for (int i = 0; i < numPlanets; i++) {
            dist = 4000 + i * 2200.0; // HACK: magic numbers
            angle = -M_PI * 0.2 + i * M_PI * 0.05; // HACK: magic numbers
            CGPoint planetLoc = CGPointMake(dist * cos(angle), dist * sin(angle));
            // HACK: giving the planet an ID of -1 means "create yourself anew, instead of from an existing planet row in the database"
            
            WOPlanet* planet;
            if (createNewSystem) {
                planet = [[[WOPlanet alloc] initAtPoint:planetLoc withId:-1] autorelease];
            } else {
                planet = [[[WOPlanet alloc] initAtPoint:planetLoc withPlist:[planetPlists objectAtIndex:i]] autorelease];
            }
            
            [self.star.translation addSublayer:planet.translation];
            [self.planets addObject:planet];
        }
    }
    return self;
}

- (NSArray*) getPlanetsFromServer
{
    NSLog(@"Getting planets from server!");
    
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
        
        NSData* data = [NSData dataWithBytes:[response cStringUsingEncoding:NSUTF8StringEncoding] 
                                      length:[response lengthOfBytesUsingEncoding:NSUTF8StringEncoding]];
        planetsPlist = [NSPropertyListSerialization propertyListWithData:data
                                                                 options:0
                                                                  format:NULL
                                                                   error:NULL];
    }
    return planetsPlist;
}

@end
