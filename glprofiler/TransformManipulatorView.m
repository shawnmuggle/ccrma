//
//  TransformManipulatorView.m
//  GLProfiler
//
//  Created by handley on 9/28/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "TransformManipulatorView.h"
#import "events.h"
#import <OpenGL/glu.h>
#import "Camera.h"
#import "Event.h"
#import "MatrixEvents.h"

void InvertMatrix(float *result, float *matrix)
{
	float det =
	matrix[3] * matrix[6] * matrix[9] * matrix[12]-
	matrix[2] * matrix[7] * matrix[9] * matrix[12]-
	matrix[3] * matrix[5] * matrix[10] * matrix[12]+
	matrix[1] * matrix[7] * matrix[10] * matrix[12]+
	matrix[2] * matrix[5] * matrix[11] * matrix[12]-
	matrix[1] * matrix[6] * matrix[11] * matrix[12]-
	matrix[3] * matrix[6] * matrix[8] * matrix[13]+
	matrix[2] * matrix[7] * matrix[8] * matrix[13]+
	matrix[3] * matrix[4] * matrix[10] * matrix[13]-
	matrix[0] * matrix[7] * matrix[10] * matrix[13]-
	matrix[2] * matrix[4] * matrix[11] * matrix[13]+
	matrix[0] * matrix[6] * matrix[11] * matrix[13]+
	matrix[3] * matrix[5] * matrix[8] * matrix[14]-
	matrix[1] * matrix[7] * matrix[8] * matrix[14]-
	matrix[3] * matrix[4] * matrix[9] * matrix[14]+
	matrix[0] * matrix[7] * matrix[9] * matrix[14]+
	matrix[1] * matrix[4] * matrix[11] * matrix[14]-
	matrix[0] * matrix[5] * matrix[11] * matrix[14]-
	matrix[2] * matrix[5] * matrix[8] * matrix[15]+
	matrix[1] * matrix[6] * matrix[8] * matrix[15]+
	matrix[2] * matrix[4] * matrix[9] * matrix[15]-
	matrix[0] * matrix[6] * matrix[9] * matrix[15]-
	matrix[1] * matrix[4] * matrix[10] * matrix[15]+
	matrix[0] * matrix[5] * matrix[10] * matrix[15];

	if (det == 0)
	{
		return;
	}
	
	result[0] = (matrix[6]*matrix[11]*matrix[13] -
		matrix[7]*matrix[10]*matrix[13] +
		matrix[7]*matrix[9]*matrix[14] -
		matrix[5]*matrix[11]*matrix[14] -
		matrix[6]*matrix[9]*matrix[15] +
		matrix[5]*matrix[10]*matrix[15])/det;
	result[1] = (matrix[3]*matrix[10]*matrix[13] -
		matrix[2]*matrix[11]*matrix[13] -
		matrix[3]*matrix[9]*matrix[14] +
		matrix[1]*matrix[11]*matrix[14] +
		matrix[2]*matrix[9]*matrix[15] -
		matrix[1]*matrix[10]*matrix[15])/det;
	result[2] = (matrix[2]*matrix[7]*matrix[13] -
		matrix[3]*matrix[6]*matrix[13] +
		matrix[3]*matrix[5]*matrix[14] -
		matrix[1]*matrix[7]*matrix[14] -
		matrix[2]*matrix[5]*matrix[15] +
		matrix[1]*matrix[6]*matrix[15])/det;
	result[3] = (matrix[3]*matrix[6]*matrix[9] -
		matrix[2]*matrix[7]*matrix[9] -
		matrix[3]*matrix[5]*matrix[10] +
		matrix[1]*matrix[7]*matrix[10] +
		matrix[2]*matrix[5]*matrix[11] -
		matrix[1]*matrix[6]*matrix[11])/det;
	result[4] = (matrix[7]*matrix[10]*matrix[12] -
		matrix[6]*matrix[11]*matrix[12] -
		matrix[7]*matrix[8]*matrix[14] +
		matrix[4]*matrix[11]*matrix[14] +
		matrix[6]*matrix[8]*matrix[15] -
		matrix[4]*matrix[10]*matrix[15])/det;
	result[5] = (matrix[2]*matrix[11]*matrix[12] -
		matrix[3]*matrix[10]*matrix[12] +
		matrix[3]*matrix[8]*matrix[14] -
		matrix[0]*matrix[11]*matrix[14] -
		matrix[2]*matrix[8]*matrix[15] +
		matrix[0]*matrix[10]*matrix[15])/det;
	result[6] = (matrix[3]*matrix[6]*matrix[12] -
		matrix[2]*matrix[7]*matrix[12] -
		matrix[3]*matrix[4]*matrix[14] +
		matrix[0]*matrix[7]*matrix[14] +
		matrix[2]*matrix[4]*matrix[15] -
		matrix[0]*matrix[6]*matrix[15])/det;
	result[7] = (matrix[2]*matrix[7]*matrix[8] -
		matrix[3]*matrix[6]*matrix[8] +
		matrix[3]*matrix[4]*matrix[10] -
		matrix[0]*matrix[7]*matrix[10] -
		matrix[2]*matrix[4]*matrix[11] +
		matrix[0]*matrix[6]*matrix[11])/det;
	result[8] = (matrix[5]*matrix[11]*matrix[12] -
		matrix[7]*matrix[9]*matrix[12] +
		matrix[7]*matrix[8]*matrix[13] -
		matrix[4]*matrix[11]*matrix[13] -
		matrix[5]*matrix[8]*matrix[15] +
		matrix[4]*matrix[9]*matrix[15])/det;
	result[9] = (matrix[3]*matrix[9]*matrix[12] -
		matrix[1]*matrix[11]*matrix[12] -
		matrix[3]*matrix[8]*matrix[13] +
		matrix[0]*matrix[11]*matrix[13] +
		matrix[1]*matrix[8]*matrix[15] -
		matrix[0]*matrix[9]*matrix[15])/det;
	result[10] = (matrix[1]*matrix[7]*matrix[12] -
		matrix[3]*matrix[5]*matrix[12] +
		matrix[3]*matrix[4]*matrix[13] -
		matrix[0]*matrix[7]*matrix[13] -
		matrix[1]*matrix[4]*matrix[15] +
		matrix[0]*matrix[5]*matrix[15])/det;
	result[11] = (matrix[3]*matrix[5]*matrix[8] -
		matrix[1]*matrix[7]*matrix[8] -
		matrix[3]*matrix[4]*matrix[9] +
		matrix[0]*matrix[7]*matrix[9] +
		matrix[1]*matrix[4]*matrix[11] -
		matrix[0]*matrix[5]*matrix[11])/det;
	result[12] = (matrix[6]*matrix[9]*matrix[12] -
		matrix[5]*matrix[10]*matrix[12] -
		matrix[6]*matrix[8]*matrix[13] +
		matrix[4]*matrix[10]*matrix[13] +
		matrix[5]*matrix[8]*matrix[14] -
		matrix[4]*matrix[9]*matrix[14])/det;
	result[13] = (matrix[1]*matrix[10]*matrix[12] -
		matrix[2]*matrix[9]*matrix[12] +
		matrix[2]*matrix[8]*matrix[13] -
		matrix[0]*matrix[10]*matrix[13] -
		matrix[1]*matrix[8]*matrix[14] +
		matrix[0]*matrix[9]*matrix[14])/det;
	result[14] = (matrix[2]*matrix[5]*matrix[12] -
		matrix[1]*matrix[6]*matrix[12] -
		matrix[2]*matrix[4]*matrix[13] +
		matrix[0]*matrix[6]*matrix[13] +
		matrix[1]*matrix[4]*matrix[14] -
		matrix[0]*matrix[5]*matrix[14])/det;
	result[15] = (matrix[1]*matrix[6]*matrix[8] -
		matrix[2]*matrix[5]*matrix[8] +
		matrix[2]*matrix[4]*matrix[9] -
		matrix[0]*matrix[6]*matrix[9] -
		matrix[1]*matrix[4]*matrix[10] +
		matrix[0]*matrix[5]*matrix[10])/det;
}


void MultVectorAndMatrix(float *result, float *vec, float *matrix)
{
	for (uint32_t i=0;i<4;++i)
	{
		result[i] = 0.0f;
		for (uint32_t j=0;j<4;++j)
		{
			result[i] += vec[j] * matrix[j*4 + i];
		}
	}
}

void Identity(float *mat)
{
	mat[0] = 1.0f;
	mat[1] = 0.0f;
	mat[2] = 0.0f;
	mat[3] = 0.0f;
	mat[4] = 0.0f;
	mat[5] = 1.0f;
	mat[6] = 0.0f;
	mat[7] = 0.0f;
	mat[8] = 0.0f;
	mat[9] = 0.0f;
	mat[10] = 1.0f;
	mat[11] = 0.0f;
	mat[12] = 0.0f;
	mat[13] = 0.0f;
	mat[14] = 0.0f;
	mat[15] = 1.0f;
}
void CopyMat(float *dest, float *src)
{
	memcpy(dest,src,sizeof(float[16]));
}
void MultMat(float *dest, float *a, float *b)
{
	memset(dest,0,sizeof(float[16]));
	
	for (int j=0;j<4;++j)
		for (int i=0;i<4;++i)
			for (int k=0;k<4;++k)
				dest[i + j*4] += a[i + k*4] * b[k + j*4];
}

@implementation TransformManipulatorView
+ (void)initialize
{
	[self exposeBinding:@"events"];
}
- (void)awakeFromNib
{
	[self bind:@"events" toObject:controller withKeyPath:@"selectedObjects" options:[NSDictionary dictionary]];
}
- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format
{
	if (self = [super initWithFrame:frameRect pixelFormat:format])
	{
		camera = [[TrackballCamera alloc] init];
	}
	return self;
}
- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super initWithCoder:aDecoder])
	{
		camera = [[TrackballCamera alloc] init];
	}
	return self;
}
- (void)dealloc
{
	[camera release];
	[super dealloc];
}
- (void)setMode:(uint32_t)inMode
{
	mode = inMode;
}

- (void)updateMatrix
{
	Identity(projectionMatrix);
	Identity(modelviewMatrix);
	
	float *matrix = modelviewMatrix;

	for (Event *e in events)
	{
		if ([e isKindOfClass:[MatrixModeEvent class]])
		{
			MatrixModeEvent *mme = (MatrixModeEvent *)e;
			self.mode = mme.mode;
			if (mme.mode == GL_PROJECTION)
			{
				matrix = projectionMatrix;
			}
			else
			{
				matrix = modelviewMatrix;
			}
		}
		else if ([e conformsToProtocol:@protocol(MatrixManipulator)])
		{
			if (!cumulativeMode)
			{
				Identity(matrix);
			}
			Event<MatrixManipulator> *emm = (Event<MatrixManipulator> *)e;
			[emm modifyMatrix:matrix];
		}
	}
	
	[self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect
{
	[[self openGLContext] makeCurrentContext];

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	float aspect = [self frame].size.width / [self frame].size.height;

	if (self.mode == GL_PROJECTION)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, aspect, 0.1f, 1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		[camera setLookAtX:0 y:0 z:0];
		[camera set];

		//find the frustum points in view-space. Draw them.
		float points[8][4] = {
			{-1,-1,0,1},
			{1,-1,0,1},
			{1,1,0,1},
			{-1,1,0,1},
			{-1,-1,1,1},
			{1,-1,1,1},
			{1,1,1,1},
			{-1,1,1,1}
		};
		float transformedPoints[8][4];

		float invProj[16];
		InvertMatrix(invProj,projectionMatrix);
		
		for (uint32_t i=0;i<8;++i)
		{
			MultVectorAndMatrix(transformedPoints[i],points[i],invProj);
			for (uint32_t j=0;j<4;++j)
				transformedPoints[i][j] /= transformedPoints[i][3];
		}
		
		int edges[][2] = {
			{0,1},{1,2},{2,3},{3,0},
			{4,5},{5,6},{6,7},{7,4},
			{0,4},{1,5},{2,6},{3,7}
		};
		
		glBegin(GL_LINES);
			for (uint32_t i=0;i<12;++i)
			{
				for (uint32_t j=0;j<2;++j)
				{
					float c = points[edges[i][j]][2] * 0.5f + 0.5f;
					glColor3f(c,c,c);
					glVertex3fv(transformedPoints[edges[i][j]]);
				}
			}
			
			glColor3f(1,0,0);
			glVertex3f(0,0,0);
			glVertex3f(.5,0,0);
			
			glColor3f(0,1,0);
			glVertex3f(0,0,0);
			glVertex3f(0,.5,0);
			
			glColor3f(0,0,1);
			glVertex3f(0,0,0);
			glVertex3f(0,0,.5);
		glEnd();
	}
	else if (mode == GL_MODELVIEW)
	{
		//setup camera
		float x = modelviewMatrix[12];
		float y = modelviewMatrix[13];
		float z = modelviewMatrix[14];
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, aspect, 0.1f, 1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		[camera setLookAtX:x/2 y:y/2 z:z/2];
		[camera set];

		//draw an origin
		glBegin(GL_LINES);
			glColor3f(0.5f,0,0);
			glVertex3f(0,0,0);
			glVertex3f(1.0f,0,0);

			glColor3f(0,0.5f,0);
			glVertex3f(0,0,0);
			glVertex3f(0,1.0f,0);

			glColor3f(0,0,0.5f);
			glVertex3f(0,0,0);
			glVertex3f(0,0,1.0f);
		glEnd();
		
		//draw the new origin
		glPushMatrix();
			glMultMatrixf(modelviewMatrix);
		
			glBegin(GL_LINES);
				glColor3f(1.0f,0,0);
				glVertex3f(0,0,0);
				glVertex3f(1.0f,0,0);

				glColor3f(0,1.0f,0);
				glVertex3f(0,0,0);
				glVertex3f(0,1.0f,0);

				glColor3f(0,0,1.0f);
				glVertex3f(0,0,0);
				glVertex3f(0,0,1.0f);
			glEnd();
		glPopMatrix();
		
		//draw an arrow, showing the difference
		glBegin(GL_LINES);
			glColor3f(0,0,0);
			glVertex3f(0,0,0);
			
			glColor3f(1,1,1);
			glVertex3f(x,y,z);
		glEnd();
	}
	
	[[self openGLContext] flushBuffer];
}

- (void)mouseDown:(NSEvent *)event
{
	NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
	[camera setPoint:loc];
}
- (void)mouseDragged:(NSEvent *)event
{
	NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
	[camera track:loc];
	[self setNeedsDisplay:YES];
}
- (void)scrollWheel:(NSEvent *)event
{
	[camera moveForward:[event deltaY]];
	[self setNeedsDisplay:YES];
}

- (IBAction)setCumulative:(id)sender
{
	cumulativeMode = [sender intValue];
	
	[self setNeedsDisplay:YES];
}

- (void)setEvents:(NSArray *)e
{
	if (e != events)
	{
		[events release];
		events = [e retain];
		
		[self updateMatrix];
	}
}

@synthesize events, mode;
@end
