/*
 *  glwrapper.c
 *  GLProfiler
 *
 *  Created by handley on 7/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "glwrapper.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

uint32_t frameCounter = 0;
uint32_t isConnected = 0;

struct SurfacePacked cachedSurface;

void FrameStartPROFILE()
{
	if (!isConnected)
	{
		isConnected = InitProfilerConnection();
		if (isConnected)
		{
			SendSurfaceEvent(cachedSurface.color, cachedSurface.depth, cachedSurface.stencil, cachedSurface.width, cachedSurface.height);
			SendCurrentGLState();
		}
	}

	if (isConnected)
	{
		SendFrameBeginEvent(frameCounter);
	}
}
void FrameEndPROFILE()
{
	if (isConnected)
	{
		SendFrameEndEvent(frameCounter);
		CloseProfilerConnection();
		isConnected = 0;
	}
	frameCounter++;
}
void SurfacePROFILE(uint8_t color, uint8_t depth, uint8_t stencil, uint16_t width, uint16_t height)
{
	if (isConnected)
	{
		SendSurfaceEvent(color,depth,stencil,width,height);
	}
	cachedSurface.color = color;
	cachedSurface.depth = depth;
	cachedSurface.stencil = stencil;
	cachedSurface.width = width;
	cachedSurface.height = height;
}

void glClearColorPROFILE(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	if (isConnected)
	{
		SendClearColor(red,green,blue,alpha);
	}
	
	glClearColor(red,green,blue,alpha);
}

void glClearPROFILE(GLbitfield mask)
{
	if (isConnected)
	{
		SendClearEvent(mask);
	}
	
	glClear(mask);
}

void glViewportPROFILE(GLint x, GLint y, GLsizei width, GLsizei height)
{
	if (isConnected)
	{
		SendViewportEvent(x,y,width,height);
	}
	
	glViewport(x,y,width,height);
}

void glMatrixModePROFILE(GLenum mode)
{
	if (isConnected)
	{
		SendMatrixModeEvent(mode);
	}
	
	glMatrixMode(mode);
}
void glLoadIdentityPROFILE(void)
{
	if (isConnected)
	{
		SendLoadIdentityEvent();
	}
	
	glLoadIdentity();
}
void glTranslatefPROFILE(GLfloat x, GLfloat y, GLfloat z)
{
	if (isConnected)
	{
		SendTranslatefEvent(x,y,z);
	}

	glTranslatef(x,y,z);
}
void glOrthoPROFILE(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	if (isConnected)
	{
		SendOrthoEvent(left,right,bottom,top,zNear,zFar);
	}
		
	glOrtho(left, right, bottom, top, zNear, zFar);
}
void gluPerspectivePROFILE(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	if (isConnected)
	{
		SendPerspectiveEvent(fovy,aspect,zNear,zFar);
	}

	gluPerspective(fovy,aspect,zNear,zFar);
}

void glGenBuffersPROFILE(GLsizei n, GLuint *buffers)
{
	glGenBuffers(n,buffers);
	
	if (isConnected)
	{
		SendGenBuffersEvent(n,buffers);
	}
}

void glBindBufferPROFILE(GLenum target, GLuint buffer)
{
	if (isConnected)
	{
		SendBindBufferEvent(target,buffer);
	}
	
	glBindBuffer(target,buffer);
}

void glBufferDataPROFILE(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	if (isConnected)
	{
		SendBufferDataEvent(target,size,data,usage);
		uint32_t bufferID;
		glGetIntegerv(target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *)&bufferID);
	}
	
	glBufferData(target,size,data,usage);
}
void glBufferSubDataPROFILE(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
	if (isConnected)
	{
		SendBufferSubDataEvent(target,offset,size,data);
	}
	glBufferSubData(target,offset,size,data);
}
void glDeleteBuffersPROFILE(GLsizei n, const GLuint *buffers)
{
	if (isConnected)
	{
		SendDeleteBuffersEvent(n,buffers);
	}

	if (shouldDelayDeletions)
	{
		AddDeleteBuffer(n,buffers);
	}
	else
	{
		//we shouldn't delete when we are connected, as that means we might not be able to later map the buffer to get at the data.
		glDeleteBuffers(n,buffers);
	}
}

void glEnableClientStatePROFILE(GLenum array)
{
	if (isConnected)
	{
		SendEnableClientStateEvent(array);
	}
	
	glEnableClientState(array);
}

void glVertexPointerPROFILE(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (isConnected)
	{
		SendVertexPointerEvent(size,type,stride,pointer);
	}
	
	glVertexPointer(size,type,stride,pointer);
}

void glColorPointerPROFILE(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (isConnected)
	{
		SendColorPointerEvent(size,type,stride,pointer);
	}

	glColorPointer(size,type,stride,pointer);
}

void glTexCoordPointerPROFILE(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (isConnected)
	{
		SendTexCoordPointerEvent(size,type,stride,pointer);
	}

	glTexCoordPointer(size,type,stride,pointer);
}

void glNormalPointerPROFILE(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (isConnected)
	{
		SendNormalPointerEvent(type,stride,pointer);
	}

	glNormalPointer(type,stride,pointer);
}


void glDrawArraysPROFILE(GLenum mode, GLint first, GLsizei count)
{
	if (isConnected)
	{
		SendDrawArraysEvent(mode,first,count);
	}
	
	glDrawArrays(mode,first,count);
}