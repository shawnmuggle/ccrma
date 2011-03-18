//
//  WOPlanet.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

@interface WOPlanet : NSObject {
    
    UIColor* color;
    float baseRadius;
    float maxHeight;
    
    NSMutableSet* trees;
    
    CGMutablePathRef path;
    CALayer* translation;
    CALayer* rotation;
    CAShapeLayer* layer;
    
    CGPoint loc;
    
    float c1, c2, c3, c4;
    float a1, a2, a3, a4;
    float r, g, b;
    NSArray* soundFiles;
    
    float rotationAngle;
    float rotationAngleIncrement;
    
    
    int planetId;
}

@property (nonatomic, retain) NSMutableSet* trees;
@property (nonatomic, retain) CAShapeLayer* layer;
@property (nonatomic, retain) CALayer* translation;
@property (nonatomic, retain) CALayer* rotation;
@property float baseRadius;
@property CGPoint loc;
@property float rotationAngle;
@property float rotationAngleIncrement;

- (id) initAtPoint:(CGPoint)newLoc;
- (id) initAtPoint:(CGPoint)newLoc withId:(int)newPlanetId;
- (id) initAtPoint:(CGPoint)newLoc withPlist:(NSDictionary*)plist;
- (void) determineShape;
- (float) surfaceFunction:(float) x;
- (void) addTreeAtAngle:(float)angle treeType:(int)type;
- (void) tick;
- (void) generatePath;
- (void) setupLayer;
- (CGColorRef) generateSquaresPatternFromColorComponents:(float*)components;
- (void) stopGrowingTree;
- (int) getNewPlanetIdFromServer;
- (void) submitPlanetInfoToServer;
- (NSArray*) getTreesFromServer;

@end

@interface WOStar : WOPlanet {
    
}

@end