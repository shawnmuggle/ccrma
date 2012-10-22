//
//  Camera.m
//  GLProfiler
//
//  Created by handley on 9/30/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "Camera.h"
#import <OpenGL/glu.h>

@implementation Camera
- (id)init
{
	if (self = [super init])
	{
		trackSpeed = 1.0f;
		movementSpeed = 0.05f;
	}
	return self;
}
- (void)moveForward:(float)scale {}
- (void)moveBackward:(float)scale {}
- (void)moveLeft:(float)scale {}
- (void)moveRight:(float)scale {}

- (void)setPoint:(NSPoint)newPoint
{
	point = newPoint;
}
- (void)track:(NSPoint)newPoint
{
	//update phi, theta
	float dx = newPoint.x - point.x, dy = newPoint.y - point.y;
	
	theta += dx * trackSpeed;
	phi += dy * trackSpeed;
	if (phi > 85)
		phi = 85;
	else if (phi < -85)
		phi = -85;
		
	point = newPoint;
}
- (void)set {}
@end


@implementation TrackballCamera
- (id)init
{
	if (self = [super init])
	{
		distanceAway = 7.0f;
	}
	return self;
}
- (void)moveForward:(float)scale
{
	distanceAway -= scale*movementSpeed*(distanceAway/10.0f);
	if (distanceAway < 1.0f)
		distanceAway = 1.0f;
	else if (distanceAway > 1000.0f)
		distanceAway = 1000.0f;
}
- (void)moveBackward:(float)scale
{
	distanceAway += scale*movementSpeed*(distanceAway/10.0f);;
	if (distanceAway < 1.0f)
		distanceAway = 1.0f;
	else if (distanceAway > 1000.0f)
		distanceAway = 1000.0f;
}
- (void)set
{
	//assume the modelview matrix is already chosen, and identity
	float x = distanceAway * sinf(3.1415f / 180.0f * -theta) * cosf(3.1415f / 180.0f * -phi),y = distanceAway * sinf(3.1415f / 180.0f * -phi),z = distanceAway * cosf(3.1415f / 180.0f * -theta) * cosf(3.1415f / 180.0f * -phi);
	gluLookAt(x, y, z, lx, ly, lz, 0, 1, 0);
}
- (void)setLookAtX:(float)inX y:(float)inY z:(float)inZ
{
	lx = inX;
	ly = inY;
	lz = inZ;
}
@end