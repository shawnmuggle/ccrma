//
//  WOTurtle.h
//  wold
//
//  Created by Michael Rotondo on 2/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "mo_gfx.h"

#import <stack>

@class WOTurtleState;

struct TurtleState {
    float x;
    float y;
    float z;
    //float xAngle;
    float yAngle;
    float zAngle;
};

@interface WOTurtle : NSObject {

    //NSMutableArray* states;
    std::stack<TurtleState*>* states;
    GLfloat* points;
    int pointsIndex;

}

//@property (retain) NSMutableArray* states;
@property std::stack<TurtleState*>* states;

- (id)initWithPoints:(GLfloat*)points;
- (void)addPoint:(Vector3D)point;
- (void) addZAngle:(GLfloat)angle;
- (void) addYAngle:(GLfloat)angle;
- (TurtleState*)currentState;
- (void) pushState;
- (void) popState;

@end


@interface WOTurtleState : NSObject <NSCopying> {

    Vector3D* vertex;
    float xAngle;
    float yAngle;
    float zAngle;
    
}

@property Vector3D* vertex;
@property float xAngle;
@property float yAngle;
@property float zAngle;

- (id)initWithVertex:(Vector3D)newVertex andXAngle:(float)newXAngle andYAngle:(float)newYAngle andZAngle:(float)newZAngle;
- (id)copyWithZone:(NSZone *)zone;

@end