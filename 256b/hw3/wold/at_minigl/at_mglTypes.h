/*
 *  at_mglTypes.h
 *  AngryTruck
 *
 *  Created by njb on 2/28/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __MGLTYPES_H__
#define __MGLTYPES_H__

typedef int           MGLint;
typedef unsigned char MGLbyte;
typedef unsigned int  MGLsize;
typedef unsigned int  MGLpixel;
typedef float         MGLfloat;
typedef bool          MGLbool;
typedef unsigned int  MGLtex_id;
typedef enum {
    MGL_MODELVIEW,
    MGL_PROJECTION
} MGLmatrix_mode;

typedef enum {
    MGL_TRIANGLES,
    MGL_QUADS
} MGLpoly_mode;

typedef enum {
	MGL_PHONG,
	MGL_TOON
} MGLshading_mode;

typedef enum {
	MGL_LIGHT0 = 0,
	MGL_LIGHT1,
	MGL_LIGHT2,
	MGL_LIGHT_NUM
} MGLlight;

typedef enum {
	MGL_LIGHT_AMBIENT,
	MGL_LIGHT_DIFFUSE,
	MGL_LIGHT_SPECULAR,
	MGL_LIGHT_POSITION
} MGLlight_param;

typedef enum {
	MGL_MAT_AMBIENT,
	MGL_MAT_DIFFUSE,
	MGL_MAT_SPECULAR,
	MGL_MAT_SHININESS
} MGLmat_param;

typedef enum {
	MGL_TEX_DIFFUSE,
	MGL_TEX_SPECULAR,
	MGL_TEX_SLOT_NUM
} MGLtex_slot;

/**
 * Macros to get and set color values from an individual
 * pixel.
 */
#define MGL_GET_RED(P)   (((P) & 0x000000ff))
#define MGL_GET_GREEN(P) (((P) & 0x0000ff00) >> 8)
#define MGL_GET_BLUE(P)  (((P) & 0x00ff0000) >> 16)

#define MGL_SET_RED(P, C)   (P = (((P) & 0xffffff00) | (C)))
#define MGL_SET_GREEN(P, C) (P = (((P) & 0xffff00ff) | ((C) << 8)))
#define MGL_SET_BLUE(P, C)  (P = (((P) & 0xff00ffff) | ((C) << 16)))


#endif