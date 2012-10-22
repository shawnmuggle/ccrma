/*
 *  events.h
 *  GLProfiler
 *
 *  Created by handley on 7/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <OpenGL/OpenGL.h>

enum EventTypes
{
	//meta events
	etCAPTURE,
	etSTOP_CAPTURE,
	etFINISHED_CAPTURING,
	etFRAME_START,
	etFRAME_END,
	etSURFACE_EVENT,
	etBUFFER_DATA_REQUEST,
	etBUFFER_DATA_RESPONSE,
	
	//gl events
	etCLEAR_COLOR,
	etVIEPORT,
	etCLEAR,
	etMATRIX_MODE,
	etLOAD_IDENTITY,
	etTRANSLATEF,
	etORTHO,
	etPERSPECTIVE,
	etGEN_BUFFERS,
	etDELETE_BUFFERS,
	etBIND_BUFFER,
	etBUFFER_DATA,
	etBUFFER_SUB_DATA,
	etENABLE_CLIENT_STATE,
	etVERTEX_POINTER,
	etCOLOR_POINTER,
	etTEXCOORD_POINTER,
	etNORMAL_POINTER,
	etDRAW_ARRAYS,
	
	//requests/replies
	etINFO_BUFFER,
	
	//bookkeeping
	MAX_EVENTS
};

struct EventHeader
{
	uint64_t fake : 1;
	uint64_t time : 63;
};

struct FramePacked
{
	struct EventHeader header;
	uint32_t frameCount;
};

struct SurfacePacked
{
	struct EventHeader header;
	uint16_t width, height;
	uint8_t color,depth,stencil;
};

struct ClearColorPacked
{
	struct EventHeader header;
	uint32_t color;
};

struct ClearPacked
{
	struct EventHeader header;
	uint32_t mask;
};

struct ViewportPacked
{
	struct EventHeader header;
	int32_t x,y;
	uint32_t width,height;
};

struct MatrixModePacked
{
	struct EventHeader header;
	uint32_t mode;
};

struct LoadIdentityPacked
{
	struct EventHeader header;
};

struct TranslatefPacked
{
	struct EventHeader header;
	float x,y,z;
};

struct OrthoPacked
{
	struct EventHeader header;
	float left,right;
	float bottom,top;
	float zNear,zFar;
};

struct PerspectivePacked
{
	struct EventHeader header;
	float fovy;
	float aspect;
	float zNear, zFar;
};

struct GenDeleteBuffersPacked
{
	struct EventHeader header;
	GLsizei count;
	//GLuint *buffers; stored after this class
};

struct BindBufferPacked
{
	struct EventHeader header;
	uint32_t target, buffer;
};

struct BufferDataPacked
{
	struct EventHeader header;
	uint32_t target, size, usage;
};

struct BufferInfoPacked
{
	struct BufferDataPacked header;
	uint32_t buffer;
};


struct BufferSubDataPacked
{
	struct EventHeader header;
	uint32_t target, offset, size;
	//void *data; stored after this class
};

struct EnableClientStatePacked
{
	struct EventHeader header;
	uint32_t array;
};

struct PointerPacked
{
	struct EventHeader header;
	uint32_t size, type, stride;
	uintptr_t pointer;
};

struct DrawArraysPacked
{
	struct EventHeader header;
	uint16_t mode;
	uint32_t first, count;
};

struct BufferDataMetaPacked
{
	struct EventHeader header;
	uint16_t bufferID;
};

void SendFrameBeginEvent(uint32_t frameCount);
void SendFrameEndEvent(uint32_t frameCount);
void SendSurfaceEvent(uint8_t color, uint8_t depth, uint8_t stencil, uint16_t width, uint16_t height);
void SendCurrentGLState();
void SendClearColor(float red, float green, float blue, float alpha);
void SendClearEvent(uint32_t mask);
void SendViewportEvent(int x, int y, unsigned int w, unsigned int h);
void SendMatrixModeEvent(uint32_t mode);
void SendLoadIdentityEvent();
void SendTranslatefEvent(float x, float y, float z);
void SendOrthoEvent(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void SendPerspectiveEvent(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
void SendGenBuffersEvent(GLsizei n, GLuint *buffers);
void SendFakeGenBuffersEvent(GLsizei n, GLuint *buffers);
void SendDeleteBuffersEvent(GLsizei n, const GLuint *buffers);
void SendBindBufferEvent(GLenum target, GLuint buffer);
void SendBufferDataEvent(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
void SendBufferSubDataEvent(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void SendFakeBufferDataEvent(GLenum target, GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage);
void SendEnableClientStateEvent(uint32_t array);
void SendVertexPointerEvent(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void SendColorPointerEvent(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void SendTexCoordPointerEvent(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void SendNormalPointerEvent(GLenum type, GLsizei stride, const GLvoid *pointer);
void SendDrawArraysEvent(GLenum mode, GLint first, GLsizei count);
#endif
