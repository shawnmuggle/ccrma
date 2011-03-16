//
//  WOState.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WOPlanet.h"
#import "WOSolarSystem.h"

@interface WOState : NSObject {
    
    // HACK: This is probably not the best way to do this, but git'er done!
    IBOutlet UIView* view;
    
    NSMutableSet* solarSystems;
    WOSolarSystem* currentSolarSystem;
    WOPlanet* currentPlanet;    
    
    int zoomLevel;
    
    float rotationAngle, rotationAngleIncrement, rotationDamping;
    
    float zoomedPlanetYOffset;
    BOOL justZoomed;
    
    CGRect screen;
    
    CATransform3D scaledAndTranslated;
}

@property (nonatomic, retain) NSMutableSet* solarSystems;
@property (nonatomic, retain) WOSolarSystem* currentSolarSystem;
@property (nonatomic, retain) WOPlanet* currentPlanet;
@property int zoomLevel;
@property float rotationAngle;
@property float zoomedPlanetYOffset;
@property BOOL justZoomed;

- (void) tick:(id)sender;
- (void) zoom;
- (void) setScreenAndSky;
- (void) render;
- (void) handlePan:(id)sender;
- (void) handleRub:(id)sender;
- (void) handleLongPress:(id)sender;
- (void) handleSolarSystemTap:(id)sender;
- (void) handlePinch:(id)sender;

@end
