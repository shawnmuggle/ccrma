//
//  Camera.h
//  GLProfiler
//
//  Created by handley on 9/30/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface Camera : NSObject {
	float trackSpeed;
	float movementSpeed;
	float phi, theta;
	
	NSPoint point;
}
- (void)moveForward:(float)scale;
- (void)moveBackward:(float)scale;
- (void)moveLeft:(float)scale;
- (void)moveRight:(float)scale;
- (void)setPoint:(NSPoint)point;
- (void)track:(NSPoint)point;
- (void)set;
@end

@interface TrackballCamera  : Camera {
	float distanceAway;
	float lx,ly,lz;
}
- (void)setLookAtX:(float)x y:(float)y z:(float)z;
@end