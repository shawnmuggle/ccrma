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
    
    CGColorRef fillColor;
    CGColorRef strokeColor;
    
    float c1, c2, c3, c4;
    float a1, a2, a3, a4;
    
    float rotationAngle;
    float rotationAngleIncrement;
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
- (void) determineShape;
- (float) surfaceFunction:(float) x;
- (void) addTreeAtAngle:(float)angle treeType:(int)type;
- (void) tick;
- (void) generatePath;
- (void) setupLayer;
- (CGColorRef) generateSquaresPatternFromColorComponents:(float*)components;

@end

@interface WOStar : WOPlanet {
    
}

@end