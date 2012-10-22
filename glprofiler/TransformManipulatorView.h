//
//  TransformManipulatorView.h
//  GLProfiler
//
//  Created by handley on 9/28/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class TrackballCamera;
@interface TransformManipulatorView : NSOpenGLView {
	IBOutlet NSArrayController *controller;
	NSArray *events;
	
	TrackballCamera *camera;
	uint32_t mode; //any one of the event types
	float modelviewMatrix[16],projectionMatrix[16];
	
	BOOL cumulativeMode;
}
- (IBAction)setCumulative:(id)sender;
@property(retain) NSArray *events;
@property uint32_t mode;
@end

void InvertMatrix(float *result, float *matrix);
void MultVectorAndMatrix(float *result, float *vec, float *matrix);
void Identity(float *mat);
void CopyMat(float *dest, float *src);
void MultMat(float *dest, float *a, float *b);