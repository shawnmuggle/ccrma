//
//  WOState.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WOPlanet.h"


@interface WOState : NSObject {
    
    NSMutableSet* planets;
    WOPlanet* currentPlanet;
    int zoomLevel;
    
    float rotationAngle, rotationAngleIncrement, rotationDamping;
    
    float zoomedPlanetYOffset;

}

@property (nonatomic, retain) NSMutableSet* planets;
@property (nonatomic, retain) WOPlanet* currentPlanet;
@property int zoomLevel;
@property float rotationAngle;
@property float zoomedPlanetYOffset;

- (void) tick:(id)sender;
- (void) handlePan:(id)sender;
- (void) handleLongPress:(id)sender;
- (void) handlePinch:(id)sender;

@end
