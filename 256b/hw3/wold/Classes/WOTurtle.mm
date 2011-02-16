//
//  WOTurtle.m
//  wold
//
//  Created by Michael Rotondo on 2/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOTurtle.h"


@implementation WOTurtle
@synthesize states;

- (id) initWithPoints:(GLfloat*)newPoints
{
    self = [super init];
    if (self) {
        //self.states = [[[NSMutableArray alloc] initWithCapacity:1000] autorelease];
        self.states = new std::stack<TurtleState*>();
        points = newPoints;
        
        //Vector3D position;
        //WOTurtleState* state = [[WOTurtleState alloc] initWithVertex:position andXAngle:0 andYAngle:0 andZAngle:0];
        //[self.states addObject:state];
        
        self.states->push(new TurtleState());
        
        pointsIndex = 0;
    }
    return self;
}

- (void) addPoint:(Vector3D)point
{
    //NSLog(@"Adding point: %f, %f", point.x, point.y);
    
    if (pointsIndex == 0) {
        // First point being added
        points[pointsIndex * 3] = point.x;
        points[pointsIndex * 3 + 1] = point.y;
        points[pointsIndex * 3 + 2] = point.z;
    } else if (pointsIndex == 1) {
        // Second point being added, don't copy previous point
        points[pointsIndex * 3] = point.x;
        points[pointsIndex * 3 + 1] = point.y;
        points[pointsIndex * 3 + 2] = point.z;
    } else {
        points[pointsIndex * 3] = points[(pointsIndex - 1) * 3];
        points[pointsIndex * 3 + 1] = points[(pointsIndex - 1) * 3 + 1];
        points[pointsIndex * 3 + 2] = points[(pointsIndex - 1) * 3 + 2];
        pointsIndex++;
        points[pointsIndex * 3] = point.x;
        points[pointsIndex * 3 + 1] = point.y;
        points[pointsIndex * 3 + 2] = point.z;
    }
    
    TurtleState* state = [self currentState];
    state->x = point.x;
    state->y = point.y;
    state->z = point.z;
    
    pointsIndex++;
}

- (void) addZAngle:(GLfloat)angle
{
    [self currentState]->zAngle += angle;
}

- (void) addYAngle:(GLfloat)angle
{
    [self currentState]->yAngle += angle;
}


- (TurtleState*) currentState
{
    return states->top();
    //return [states lastObject];
}

- (void) pushState
{
    TurtleState* currentState = [self currentState];
    TurtleState* newState = new TurtleState();
    newState->x = currentState->x;
    newState->y = currentState->y;
    newState->z = currentState->z;
    newState->zAngle = currentState->zAngle;
    newState->yAngle = currentState->yAngle;
    
    self.states->push(newState);
    //[self.states addObject:[[self currentState] copy]];
}

- (void) popState
{
    TurtleState* state = self.states->top();
    self.states->pop();
    delete state;
    //[self.states removeLastObject];
}

- (void) dealloc
{
    //[states release];
    delete states;
    
    [super dealloc];
}

@end

@implementation WOTurtleState
@synthesize vertex, xAngle, yAngle, zAngle;

- (id) initWithVertex:(Vector3D)newVertex andXAngle:(float)newXAngle andYAngle:(float)newYAngle andZAngle:(float)newZAngle
{
    self = [super init];
    if (self) {
        self.vertex = new Vector3D(newVertex);
        self.xAngle = newXAngle;
        self.yAngle = newYAngle;
        self.zAngle = newZAngle;
    }
    return self;
}

- (id) copyWithZone:(NSZone *)zone
{
    WOTurtleState* copy = [[[self class] allocWithZone:zone] initWithVertex:*(self.vertex)
                                                                  andXAngle:self.xAngle
                                                                  andYAngle:self.yAngle
                                                                  andZAngle:self.zAngle];
    return copy;
}

- (void) dealloc
{
    free(vertex);
    
    [super dealloc];
}

@end