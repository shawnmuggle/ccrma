/*
 *  glwrapper.h
 *  GLProfiler
 *
 *  Created by handley on 7/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GL_WRAPPER_H
#define GL_WRAPPER_H

#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>

#include "connection.h"
#include "utility.h"
#include "events.h"

#ifdef __cplusplus
extern "C" {
#endif

//PROFILING META CONTROL
//void InitPROFILE();
void FrameStartPROFILE();
void FrameEndPROFILE();
void SurfacePROFILE(uint8_t color, uint8_t depth, uint8_t stencil, uint16_t width, uint16_t height);

//this module replaces all of the gl* calls with gl*Profile calls, that tell the profiler what's going on.
//we will eventually support both live and offline analysis modes. Only live is supported right now.
//Also, we batch all mere state change calls (as delta over and against the last batch), and send each batch as an atomic constant over to the analysis app only for drawing commands.

//we communicate with the app via a Mach Port on Mac OS X. The communications logic is written in C so as to be as extensible as possible.

void glClearColorPROFILE(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glClearPROFILE(GLbitfield mask);
void glViewportPROFILE(GLint x, GLint y, GLsizei width, GLsizei height);

//matrix manipulation
void glMatrixModePROFILE(GLenum mode);
void glLoadIdentityPROFILE(void);
void glTranslatefPROFILE(GLfloat x, GLfloat y, GLfloat z);
void gluPerspectivePROFILE(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
void glOrthoPROFILE(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);

//buffers
void glGenBuffersPROFILE(GLsizei n, GLuint *buffers);
void glBindBufferPROFILE(GLenum target, GLuint buffer);
void glBufferDataPROFILE(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
void glBufferSubDataPROFILE(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void glDeleteBuffersPROFILE(GLsizei n, const GLuint *buffers);
//state
void glEnableClientStatePROFILE(GLenum array);

//arrays
void glVertexPointerPROFILE(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glColorPointerPROFILE(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glTexCoordPointerPROFILE(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glNormalPointerPROFILE(GLenum type, GLsizei stride, const GLvoid *pointer);

//draw events
void glDrawArraysPROFILE(GLenum mode, GLint first, GLsizei count);

#ifdef __cplusplus
}
#endif

#endif
