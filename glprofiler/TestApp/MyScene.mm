//
//  MyScene.m
//
//  Created by handley on 7/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MyScene.h"

#import "glwrapper.h"

struct SpriteParticle
{
	float position[3];
	float scale;
	uint32_t color;
};

struct SpritePrimitive
{
	struct Vertex
	{
		float position[3];
		uint32_t color;
		float normal[3];
		float uv[2];
	} vertices[4];
};
const uint32_t NUM_PARTICLES = 1024;

float frand(float max)
{
	return max * random() / (float)RAND_MAX;
}
float frand(float min, float max)
{
	return frand(max-min) + min;
}
float frand(float min, float max, uint32_t seed)
{
	srandom(seed);
	return frand(min,max);
}

@implementation MyScene
- (id)init
{
	if (self = [super init])
	{
		accTime = 0.0;
		mouseLoc.x = 100.0f;
		mouseLoc.y = 100.0f;
	}
	return self;
}
- (void)advanceTimeBy:(CFTimeInterval)dt
{
	if (dt > 1.0f)
	{
		dt = 0.0f;
	}
	accTime += dt;
	
	//calculate new particle positions
	SpriteParticle particles[NUM_PARTICLES];
	for (uint32_t i=0;i<NUM_PARTICLES;++i)
	{
		float rate = 50.0f; //particles / sec
		float localOffset = i / rate;
		float lifetime = 10.0f;
		float t = fmodf(accTime + localOffset, lifetime);
	
		srandom(i + 0xbeefca5e);
		particles[i].position[0] = mouseLoc.x + frand(-10.0f,10.0f) * t;
		particles[i].position[1] = mouseLoc.y + 10.0f * t + .5f * -10.0f * t * t;
		particles[i].position[2] = 0.0f;
		particles[i].scale = 10.0f;
		
		particles[i].color = random();
	}
	
	//generate vertex buffer
	SpritePrimitive vertices[NUM_PARTICLES];
	const float positions[4][2] = {
		{-.5f,-.5f},
		{-.5f,+.5f},
		{+.5f,+.5f},
		{+.5f,-.5f}
	};
	for (uint32_t i=0;i<NUM_PARTICLES;++i)
	{
		for (uint32_t v=0;v<4;++v)
		{
			vertices[i].vertices[v].position[0] = positions[v][0] * particles[i].scale + particles[i].position[0];
			vertices[i].vertices[v].position[1] = positions[v][1] * particles[i].scale + particles[i].position[1];
			vertices[i].vertices[v].position[2] = 0.0f;
			vertices[i].vertices[v].color = particles[i].color;
			vertices[i].vertices[v].normal[0] = 0;
			vertices[i].vertices[v].normal[1] = 0;
			vertices[i].vertices[v].normal[2] = 1;
			vertices[i].vertices[v].uv[0] = positions[v][0] + 0.5f;
			vertices[i].vertices[v].uv[1] = positions[v][1] + 0.5f;
		}
	}
	
	if (!vertexVBO)
	{
		glGenBuffersPROFILE(1,&vertexVBO);
	}
	glBindBufferPROFILE(GL_ARRAY_BUFFER, vertexVBO);
	glBufferDataPROFILE(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
- (NSRect)viewportRect
{
	return viewportRect;
}
- (void)setViewportRect:(NSRect)vr
{
	viewportRect = vr;
}
- (void)render
{
	FrameStartPROFILE();
	
	glViewportPROFILE(0, 0, viewportRect.size.width, viewportRect.size.height);

	glClearColorPROFILE(0,0,0,0);
	glClearPROFILE(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixModePROFILE(GL_PROJECTION);
	glLoadIdentityPROFILE();
	glOrthoPROFILE(NSMinX(viewportRect), NSMaxX(viewportRect), NSMinY(viewportRect), NSMaxY(viewportRect), -1, 1.0f);
	glLoadIdentityPROFILE();
	gluPerspectivePROFILE(45.0f, NSWidth(viewportRect)/NSHeight(viewportRect), .1, 100);
	glMatrixModePROFILE(GL_MODELVIEW);
	glLoadIdentityPROFILE();
	glTranslatefPROFILE(0,0,-100);

	GLuint test;
	glGenBuffersPROFILE(1, &test);
	glDeleteBuffersPROFILE(1, &test);

	glBindBufferPROFILE(GL_ARRAY_BUFFER, vertexVBO);
	glEnableClientStatePROFILE(GL_VERTEX_ARRAY);
	glEnableClientStatePROFILE(GL_COLOR_ARRAY);
	glVertexPointerPROFILE(3, GL_FLOAT, sizeof(SpritePrimitive::Vertex), 0);
	glColorPointerPROFILE(4, GL_UNSIGNED_BYTE, sizeof(SpritePrimitive::Vertex), (void*)offsetof(SpritePrimitive::Vertex,color));
	glTexCoordPointerPROFILE(2, GL_FLOAT, sizeof(SpritePrimitive::Vertex), (void *)offsetof(SpritePrimitive::Vertex,uv));
	glNormalPointerPROFILE(GL_FLOAT, sizeof(SpritePrimitive::Vertex), (void *)offsetof(SpritePrimitive::Vertex,normal));
	
	glDrawArraysPROFILE(GL_QUADS, 0, NUM_PARTICLES*4);
	
	FrameEndPROFILE();
}
@synthesize mouseLoc;
@end
