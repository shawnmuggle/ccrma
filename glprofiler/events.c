/*
 *  events.c
 *  GLProfiler
 *
 *  Created by handley on 7/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "events.h"
#include "connection.h"
#include "mytime.h"
#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint32_t validVertexBuffer = 0;

void InitializeHeader(struct EventHeader *header)
{
	header->fake = 0;
	header->time = GetHighResTime();
}

void SendFrameBeginEvent(uint32_t frameCount)
{
	struct FramePacked event;
	InitializeHeader(&event.header);
	
	event.frameCount = frameCount;
	
	SendData(&event, sizeof(event), etFRAME_START);
}
void SendFrameEndEvent(uint32_t frameCount)
{
	struct FramePacked event;
	InitializeHeader(&event.header);
	
	event.frameCount = frameCount;
	
	SendData(&event, sizeof(event), etFRAME_END);
}

void SendSurfaceEvent(uint8_t color, uint8_t depth, uint8_t stencil, uint16_t width, uint16_t height)
{
	struct SurfacePacked event;
	InitializeHeader(&event.header);
	
	event.color = color;
	event.depth = depth;
	event.stencil = stencil;
	event.width = width;
	event.height = height;
	
	SendData(&event,sizeof(event), etSURFACE_EVENT);
}
void SendClearColor(float red, float green, float blue, float alpha)
{
	struct ClearColorPacked event;
	InitializeHeader(&event.header);
	
	event.color = ConvertFloatColorTo32Bits(red, green, blue, alpha);
	
	SendData(&event,sizeof(event), etCLEAR_COLOR);
}
void SendClearEvent(uint32_t mask)
{
	struct ClearPacked event;
	InitializeHeader(&event.header);
	
	event.mask = mask;
	
	SendData(&event,sizeof(event), etCLEAR);
}
void SendViewportEvent(int x, int y, unsigned int w, unsigned int h)
{
	struct ViewportPacked event;
	InitializeHeader(&event.header);
	
	event.x = x;
	event.y = y;
	event.width = w;
	event.height = h;
	
	SendData(&event,sizeof(event), etVIEPORT);
}
void SendMatrixModeEvent(uint32_t mode)
{
	struct MatrixModePacked event;
	InitializeHeader(&event.header);
	
	event.mode = mode;
	
	SendData(&event,sizeof(event), etMATRIX_MODE);
}
void SendLoadIdentityEvent()
{
	struct LoadIdentityPacked event;
	InitializeHeader(&event.header);
	
	SendData(&event,sizeof(event), etLOAD_IDENTITY);
}
void SendTranslatefEvent(float x, float y, float z)
{
	struct TranslatefPacked event;
	InitializeHeader(&event.header);
	
	event.x = x;
	event.y = y;
	event.z = z;
	
	SendData(&event,sizeof(event), etTRANSLATEF);
}
void SendOrthoEvent(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	struct OrthoPacked event;
	InitializeHeader(&event.header);
	
	event.left = left;
	event.right = right;
	event.bottom = bottom;
	event.top = top;
	event.zNear = zNear;
	event.zFar = zFar;
	
	SendData(&event,sizeof(event), etORTHO);
}
void SendPerspectiveEvent(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	struct PerspectivePacked event;
	InitializeHeader(&event.header);
	
	event.fovy = fovy;
	event.aspect = aspect;
	event.zNear = zNear;
	event.zFar = zFar;
	
	SendData(&event,sizeof(event), etPERSPECTIVE);
}
void SendGenBuffersEvent(GLsizei n, GLuint *buffers)
{
	struct GenDeleteBuffersPacked event;
	InitializeHeader(&event.header);
	
	event.count = n;
	
	SendDataWithExtra(&event,sizeof(event), etGEN_BUFFERS, buffers, n*sizeof(GLuint));
}
void SendFakeGenBuffersEvent(GLsizei n, GLuint *buffers)
{
	struct GenDeleteBuffersPacked event;
	InitializeHeader(&event.header);
	
	event.header.fake = 1;
	event.count = n;
	
	SendDataWithExtra(&event,sizeof(event), etGEN_BUFFERS, buffers, n*sizeof(GLuint));
}
void SendDeleteBuffersEvent(GLsizei n, const GLuint *buffers)
{
	struct GenDeleteBuffersPacked event;
	InitializeHeader(&event.header);
	
	event.count = n;
	
	SendDataWithExtra(&event,sizeof(event), etDELETE_BUFFERS, buffers, n*sizeof(GLuint));
}
void SendBindBufferEvent(GLenum target, GLuint buffer)
{
	struct BindBufferPacked event;
	InitializeHeader(&event.header);
	
	event.target = target;
	event.buffer = buffer;
	
	if (target == GL_ARRAY_BUFFER)
	{
		validVertexBuffer = (buffer > 0);
	}
	
	SendData(&event,sizeof(event), etBIND_BUFFER);
}
void SendBufferDataEvent(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	struct BufferDataPacked event;
	InitializeHeader(&event.header);
	
	event.target = target;
	event.size = size;
	event.usage = usage;
	
	SendDataWithExtra(&event,sizeof(event), etBUFFER_DATA, data, size);
}
void SendBufferSubDataEvent(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
	struct BufferSubDataPacked event;
	InitializeHeader(&event.header);
	
	event.target = target;
	event.offset = offset;
	event.size = size;
	
	SendDataWithExtra(&event, sizeof(event), etBUFFER_SUB_DATA, data, size);
}
void SendFakeBufferDataEvent(GLenum target, GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	struct BufferInfoPacked event;
	InitializeHeader(&event.header.header);
	
	event.header.target = target;
	event.header.size = size;
	event.header.usage = usage;
	event.buffer = buffer;
	
	SendDataWithExtra(&event,sizeof(event), etINFO_BUFFER, data, size);
}
void SendEnableClientStateEvent(uint32_t array)
{
	struct EnableClientStatePacked event;
	InitializeHeader(&event.header);
	
	event.array = array;
	
	SendData(&event,sizeof(event), etENABLE_CLIENT_STATE);
}
void SendVertexPointerEvent(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	struct PointerPacked event;
	InitializeHeader(&event.header);
	
	event.size = size;
	event.type = type;
	event.stride = stride;
	//pointer might be actual memory, or just an offset into a buffer. If it's actual memory, then I need to send that over. If it's just an offset, then I need to send an offset. Hmm.
	if (validVertexBuffer)
	{
		event.pointer = (uintptr_t)pointer;
		SendData(&event,sizeof(event),etVERTEX_POINTER);
	}
	else
	{
		event.pointer = 0x0;
		//SendDataWithExtra(&event,sizeof(event),etVERTEX_POINTER,pointer,);
		fprintf(stderr,"glVertexPointer without VBOs is unsupported at this time!\n");
	}
}
void SendColorPointerEvent(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	struct PointerPacked event;
	InitializeHeader(&event.header);
	
	event.size = size;
	event.type = type;
	event.stride = stride;

	//pointer might be actual memory, or just an offset into a buffer. If it's actual memory, then I need to send that over. If it's just an offset, then I need to send an offset. Hmm.
	if (validVertexBuffer)
	{
		event.pointer = (uintptr_t)pointer;
		SendData(&event,sizeof(event),etCOLOR_POINTER);
	}
	else
	{
		event.pointer = 0x0;
		//SendDataWithExtra(&event,sizeof(event),etCOLOR_POINTER,pointer,);
		fprintf(stderr,"glColorPointer without VBOs is unsupported at this time!\n");
	}
}
void SendTexCoordPointerEvent(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	struct PointerPacked event;
	InitializeHeader(&event.header);
	
	event.size = size;
	event.type = type;
	event.stride = stride;

	//pointer might be actual memory, or just an offset into a buffer. If it's actual memory, then I need to send that over. If it's just an offset, then I need to send an offset. Hmm.
	if (validVertexBuffer)
	{
		event.pointer = (uintptr_t)pointer;
		SendData(&event,sizeof(event),etTEXCOORD_POINTER);
	}
	else
	{
		event.pointer = 0x0;
		//SendDataWithExtra(&event,sizeof(event),etTEXCOORD_POINTER,pointer,);
		fprintf(stderr,"glTexCoordPointer without VBOs is unsupported at this time!\n");
	}
}
void SendNormalPointerEvent(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	struct PointerPacked event;
	InitializeHeader(&event.header);
	
	event.size = 3;
	event.type = type;
	event.stride = stride;

	//pointer might be actual memory, or just an offset into a buffer. If it's actual memory, then I need to send that over. If it's just an offset, then I need to send an offset. Hmm.
	if (validVertexBuffer)
	{
		event.pointer = (uintptr_t)pointer;
		SendData(&event,sizeof(event),etNORMAL_POINTER);
	}
	else
	{
		event.pointer = 0x0;
		//SendDataWithExtra(&event,sizeof(event),etNORMAL,pointer,);
		fprintf(stderr,"glNormalPointer without VBOs is unsupported at this time!\n");
	}
}
void SendDrawArraysEvent(GLenum mode, GLint first, GLsizei count)
{
	struct DrawArraysPacked event;
	InitializeHeader(&event.header);
	
	event.mode = mode;
	event.first = first;
	event.count = count;

	SendData(&event,sizeof(event),etDRAW_ARRAYS);
}

void SendBufferInfo(GLuint buffer, unsigned int isIndexBuffer)
{
	GLenum b = isIndexBuffer ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

	//check to make sure the desired buffer is really a buffer name
	if (glIsBuffer(buffer))
	{
		//get the currently bound buffer, cache its index
		GLint oldBufferID;
		glGetIntegerv(isIndexBuffer ? GL_ELEMENT_ARRAY_BUFFER_BINDING : GL_ARRAY_BUFFER_BINDING, &oldBufferID);
		
		//set the desired buffer
		glBindBuffer(b, buffer);

		//query the state about the buffer
		GLint size, usage;
		glGetBufferParameteriv(b, GL_BUFFER_SIZE, &size);
		glGetBufferParameteriv(b, GL_BUFFER_USAGE, &usage);
		const GLvoid *data = glMapBuffer(b, GL_READ_ONLY);
		SendFakeBufferDataEvent(b, buffer, size, data, usage);
		glUnmapBuffer(b);
		
		//restore original bufferID
		glBindBuffer(b, oldBufferID);
	}
	else
	{
		//the requested buffer is NOT VALID... send a junk response back
		SendFakeBufferDataEvent(b, buffer, 0, 0, 0);
	}
}


void SendCurrentGLState()
{
	//extract any relevant GL state, send the state to the profiler
	
	//current array buffer
	GLint bufferID;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bufferID);
	if (bufferID != 0)
	{
		SendBufferInfo(bufferID,0);
	}
	
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &bufferID);
	if (bufferID != 0)
	{
		SendBufferInfo(bufferID,1);
	}
}