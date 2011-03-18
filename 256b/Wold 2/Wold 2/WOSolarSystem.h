//
//  WOSolarSystem.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "WOPlanet.h"

@interface WOSolarSystem : NSObject {
    
    WOStar* star;
    NSMutableArray* planets;

    CALayer* sky;
    CALayer* zoom;
    
    int systemId;
    
}

@property (nonatomic, retain) WOStar* star;
@property (nonatomic, retain) NSMutableArray* planets;
@property (nonatomic, retain) CALayer* sky;
@property (nonatomic, retain) CALayer* zoom;

- (id) initWithId:(int)newSystemId;
- (NSArray*) getPlanetsFromServer;

@end
