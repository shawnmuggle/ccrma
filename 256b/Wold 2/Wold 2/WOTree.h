//
//  WOTree.h
//  Wold 2
//
//  Created by Michael Rotondo on 3/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WOLSystem.h"

@interface WOTree : NSObject {
    
    float angle;
    WOLSystem* lSystem;
    
}

@property float angle;

- (id) initWithAngle:(float)newAngle;
- (void) renderAtPoint:(CGPoint)pos withAngle:(float)angle;
- (void) tick;

@end
