//
//  MatrixEvents.m
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MatrixEvents.h"
#import <OpenGL/glu.h>
#import "TransformManipulatorView.h"

@implementation MatrixModeEvent
- (uint32_t)mode
{
	return data->mode;
}
- (void)setMode:(uint32_t)mode
{
	data->mode = mode;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Matrix Mode: %s",self.mode == GL_MODELVIEW ? "MODELVIEW" : "PERSPECTIVE"];
}
- (void)drawRect:(NSRect)rect
{
	glMatrixMode(self.mode);
}
- (NSString *)viewName
{
	return @"MatrixEvent";
}
@end

@implementation LoadIdentityEvent
- (NSString *)_name
{
	return @"Load Identity";
}
- (void)drawRect:(NSRect)rect
{
	glLoadIdentity();
}
- (void)modifyMatrix:(float *)inMatrix
{
	Identity(inMatrix);
}
- (NSString *)viewName
{
	return @"MatrixEvent";
}
@end

@implementation TranslatefEvent
- (float)x
{
	return data->x;
}
- (void)setX:(float)x
{
	data->x = x;
}
- (float)y
{
	return data->y;
}
- (void)setY:(float)y
{
	data->y = y;
}
- (float)z
{
	return data->z;
}
- (void)setZ:(float)z
{
	data->z = z;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Translate (float): (%f, %f, %f)",self.x, self.y, self.z];
}
- (void)drawRect:(NSRect)rect
{
	glTranslatef(self.x, self.y, self.z);
}
- (void)modifyMatrix:(float *)inMatrix
{
	inMatrix[12] += self.x;
	inMatrix[13] += self.y;
	inMatrix[14] += self.z;
}
- (NSString *)viewName
{
	return @"MatrixEvent";
}
@end

@implementation OrthoEvent
- (float)left
{
	return data->left;
}
- (void)setLeft:(float)left
{
	data->left = left;
}
- (float)right
{
	return data->right;
}
- (void)setRight:(float)right
{
	data->right = right;
}
- (float)bottom
{
	return data->bottom;
}
- (void)setBottom:(float)bottom
{
	data->bottom = bottom;
}
- (float)top
{
	return data->top;
}
- (void)setTop:(float)top
{
	data->top = top;
}
- (float)zNear
{
	return data->zNear;
}
- (void)setZNear:(float)zNear
{
	data->zNear = zNear;
}
- (float)zFar
{
	return data->zFar;
}
- (void)setZFar:(float)zFar
{
	data->zFar = zFar;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Ortho: %.2f,%.2f, %.2f,%.2f, %.2f,%.2f",self.left,self.right,self.bottom,self.top,self.zNear,self.zFar];
}
- (void)drawRect:(NSRect)rect
{
	glOrtho(self.left, self.right, self.bottom, self.top, self.zNear, self.zFar);
}
- (void)modifyMatrix:(float *)inMatrix
{
	float m[16];
	m[0] = 2.0f/(self.right - self.left);
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
	
	m[4] = 0;
	m[5] = 2.0f/(self.top - self.bottom);
	m[6] = 0;
	m[7] = 0;
	
	m[8] = 0;
	m[9] = 0;
	m[10] = -2.0f/(self.zFar - self.zNear);
	m[11] = 0;
	
	m[12] = -(self.right + self.left) / (self.right - self.left);
	m[13] = -(self.top + self.bottom) / (self.top - self.bottom);
	m[14] = -(self.zFar + self.zNear) / (self.zFar - self.zNear);
	m[15] = 1.0f;
	
	float m2[16];
	CopyMat(m2, inMatrix);
	MultMat(inMatrix, m2, m);
}
- (NSString *)viewName
{
	return @"MatrixEvent";
}
@end

@implementation PerspectiveEvent
- (float)fovy
{
	return data->fovy;
}
- (void)setFovy:(float)fovy
{
	data->fovy = fovy;
}
- (float)aspect
{
	return data->aspect;
}
- (void)setAspect:(float)aspect
{
	data->aspect = aspect;
}
- (float)zNear
{
	return data->zNear;
}
- (void)setZNear:(float)zNear
{
	data->zNear = zNear;
}
- (float)zFar
{
	return data->zFar;
}
- (void)setZFar:(float)zFar
{
	data->zFar = zFar;
}
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Perspective: %.2f° %.2f %f,%f",self.fovy,self.aspect,self.zNear,self.zFar];
}
- (void)drawRect:(NSRect)dirtyRect
{
	gluPerspective(self.fovy, self.aspect, self.zNear, self.zFar);
}
- (void)modifyMatrix:(float *)inMatrix
{
	float f = 1.0f/tanf((3.1415926f / 180.0f * self.fovy)/2.0f);
	
	float m[16];
	m[0] = f/self.aspect;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
	
	m[4] = 0;
	m[5] = f;
	m[6] = 0;
	m[7] = 0;
	
	m[8] = 0;
	m[9] = 0;
	m[10] = (self.zFar + self.zNear)/(self.zNear - self.zFar);
	m[11] = -1;
	
	m[12] = 0;
	m[13] = 0;
	m[14] = 2*self.zFar*self.zNear/(self.zNear - self.zFar);
	m[15] = 0.0f;
	
	float m2[16];
	CopyMat(m2, inMatrix);
	MultMat(inMatrix, m2, m);
}
- (NSString *)viewName
{
	return @"MatrixEvent";
}
@end
