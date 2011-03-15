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
    CAShapeLayer* layer;
}

@property (nonatomic, retain) NSMutableSet* trees;
@property (nonatomic, retain) CAShapeLayer* layer;

- (float) surfaceFunction:(float) x;
- (void) addTreeAtAngle:(float)angle;
- (void) tick;
- (void) generatePath;

@end
