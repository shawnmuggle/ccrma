
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>
#include <vector>
#include <stack>
#include <map>
#include <math.h>
#include "at_minigl.h"
#include "at_mglMaterial.h"
#include "at_mglLight.h"
#include "at_mglUtils.h"
#include "at_mglTexture.h"
#include "at_mglUtils.h"

using namespace std;

bool mglInvertMatrix(const float * m, float * invOut);

//int MGLtexture::textureIDGenerator = 0;

// Data structures definition

struct MGLvertex {
    MGLfloat x, y, z, w;
    MGLpixel color;
	
    ///////////////////////////////////////////////////////
    //
    // TODO:
    //
    // Add new per-vertex attributes here.
    //
    ///////////////////////////////////////////////////////
    MGLfloat x_n, y_n, z_n;
    MGLfloat x_eye, y_eye, z_eye, w_eye;
    
    MGLfloat u, v;
    
    
	
    ///////////////////////////////////////////////////////
    //
    // END HERE
    //
    ///////////////////////////////////////////////////////
};

struct MGLtriangle {
    MGLvertex v[3];
};




// Global variables definition

MGLpixel curColor = 0xff000000;              
 
MGLmatrix_mode curMatrixMode = MGL_MODELVIEW;  
 
MGLpoly_mode curPolyMode = MGL_TRIANGLES;       

MGLbool hasBegun = false; 

MGLint curIndex = 0;       // used to ensure correct number of calls to mglVertex

MGLmatrix modelViewMatrix, projectionMatrix;  // store the current modelview and projection matrices

MGLmatrix * curMatrix = &modelViewMatrix;     // always point to one of the above matrix according to the current matrix mode

stack<MGLmatrix> modelViewStack;              // modelview matrix stack

stack<MGLmatrix> projectionStack;             // projection matrix stack

vector<MGLvertex> transformedVertices;  // store all vertices in clipping space
// coordinates without dividing by w


// NICK ADDED GLOBALS
MGLmatrix curNormalMatrix;
MGLfloat currentNormal[3] = {0,0,0};
MGLMaterial materials;
MGLLight lights[3] = {MGLLight(true), MGLLight(false), MGLLight(false)};
bool isLightingOn = false;


MGLtex_id currentDiffuseTextureID = 0;
MGLtex_id currentSpecularTextureID = 0;

MGLtex_slot currentTextureSlot = MGL_TEX_DIFFUSE;
MGLshading_mode currentShadingMode = MGL_PHONG;
MGLfloat currentU = 0;
MGLfloat currentV = 0;

bool diffuseTextureEnabled = false;
bool specularTextureEnabled = false;

//map<int, MGLtexture*> g_textures;




// Helper functions

/**
 * Debug utility function to display the homogeneous coordinates of a vertex.
 */
void debugShowVertex(const MGLvertex& ver, const char* msg = NULL)
{
    if (msg) fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", ver.x, ver.y, ver.z, ver.w);
    fprintf(stderr, "\n");
}

/**
 * Debug utility function to display the elements of a 4x4 matrix.
 */
void debugShowMatrix(const MGLmatrix& mat, const char* msg = NULL)
{
    if (msg) fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", mat.m[0], mat.m[4], mat.m[8], mat.m[12]);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", mat.m[1], mat.m[5], mat.m[9], mat.m[13]);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", mat.m[2], mat.m[6], mat.m[10], mat.m[14]);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", mat.m[3], mat.m[7], mat.m[11], mat.m[15]);
    fprintf(stderr, "\n");
}

/**
 * Matrix vector multiplication.
 *
 * Take vi and vo as a column vector. This function gives vo = mat * vi.
 * Input - mat, vi
 * Output - vo
 */
void mulMatrixVector(const MGLmatrix& mat, const MGLfloat* vi, MGLfloat* vo)
{
    vo[0] = mat.m[0] * vi[0] + mat.m[4] * vi[1] + mat.m[8] * vi[2] + mat.m[12] * vi[3];
    vo[1] = mat.m[1] * vi[0] + mat.m[5] * vi[1] + mat.m[9] * vi[2] + mat.m[13] * vi[3];
    vo[2] = mat.m[2] * vi[0] + mat.m[6] * vi[1] + mat.m[10] * vi[2] + mat.m[14] * vi[3];
    vo[3] = mat.m[3] * vi[0] + mat.m[7] * vi[1] + mat.m[11] * vi[2] + mat.m[15] * vi[3];
}

void mulMatrixVector(const MGLfloat* vi, MGLfloat* vo)
{
    const MGLmatrix mat = modelViewMatrix;
    vo[0] = mat.m[0] * vi[0] + mat.m[4] * vi[1] + mat.m[8] * vi[2] + mat.m[12] * vi[3];
    vo[1] = mat.m[1] * vi[0] + mat.m[5] * vi[1] + mat.m[9] * vi[2] + mat.m[13] * vi[3];
    vo[2] = mat.m[2] * vi[0] + mat.m[6] * vi[1] + mat.m[10] * vi[2] + mat.m[14] * vi[3];
    vo[3] = mat.m[3] * vi[0] + mat.m[7] * vi[1] + mat.m[11] * vi[2] + mat.m[15] * vi[3];
}


MGLfloat dot(MGLfloat *v1, MGLfloat * v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void norm(MGLfloat * v)
{
    MGLfloat norm = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    
    if(norm==0)
        return;
    
    v[0] = v[0]/norm;
    v[1] = v[1]/norm;
    v[2] = v[2]/norm;
    
}
 
  
/*************************************************************
 * Lighting/shading/texture functions (to be implemented for
 * Assignment 2).
 *************************************************************/

/**
 * Enable or disable lighting for the rendered scene.  Lighting should
 * be disabled by default.
 */
void mglLightingEnabled(bool enabled)
{
	isLightingOn = enabled;
}

/**
 * Enable or disable the specified individual light.  All lights
 * should be disabled by default.
 */
void mglLightEnabled(MGLlight light, bool enabled)
{
    if( (light < 0) || (light >2))
    {
        MGL_ERROR("[mglLightEnabled]: ERROR, incorrect light\n");
        return;
    }
    lights[light].isOn = enabled;
	
}

/**
 * Set the type of shading (Phong or toon) to be used when
 * the scene is rendered.  For Phong shading, the per-pixel lighting
 * equations should be used for specular, ambient, and diffuse lighting.
 * Toon shading is similar, except that a discretized (rather than 
 * continuous) value is used for the angle between the light vector and 
 * the normal vector.
 */
void mglShadingMode(MGLshading_mode mode)
{
    if(mode==MGL_PHONG)
        currentShadingMode = MGL_PHONG;
    else {
        currentShadingMode = MGL_TOON;
    }
}

/**
 * Set parameters for a particular light.  light specifies
 * the light whose parameters we should set, pname specifies
 * the parameter to set, and values is a pointer to the floats
 * which specify the value for that parameter.  Parameters
 * are handled as follows:
 *
 * MGL_LIGHT_AMBIENT
 * Set the ambient intensity of the light; values should point
 * to three floats representing the ambient RGB (in that order)
 * intensity of the light in homogeneous coordinates.  The initial
 * ambient intensity should be (0, 0, 0).
 *
 * MGL_LIGHT_DIFFUSE
 * Set the diffuse intensity of the light; values should point to three
 * floats representing the diffuse RGB (in that order) intensity of
 * the light in homogeneous coordinates.  The initial diffuse intenisty
 * for MGL_LIGHT0 should be (1, 1, 1); for all others, it should
 * be (0, 0, 0).
 * 
 * MGL_LIGHT_SPECULAR
 * Set the specular intensity of the light; values should be point to
 * three floats representing the specular RGB (in that order) intensity
 * of the light in homogeneous coordinates.  The initial specular
 * intensity for MGL_LIGHT0 should be (1, 1, 1); for all others,
 * it should be (0, 0, 0).
 *
 * MGL_LIGHT_POSITION
 * Set the position of the light.  This position should be transformed
 * by the modelview matrix upon specification.  values should point to
 * three floats representing the position of the light in homogeneous
 * coordinates.  The initial position should be (0, 0, 1).
 */
void mglLight(MGLlight light,
			  MGLlight_param pname,
			  MGLfloat *values)
{
    if( (light < 0) || (light >2))
    {
        MGL_ERROR("[mglLightEnabled]: ERROR, incorrect light\n");
        return;
    }
    
    if(pname==MGL_LIGHT_AMBIENT)
    {
        lights[light].ambient[0] = values[0];
        lights[light].ambient[1] = values[1];
        lights[light].ambient[2] = values[2];
    }
    else if(pname==MGL_LIGHT_DIFFUSE)
    {
        lights[light].diffuse[0] = values[0];
        lights[light].diffuse[1] = values[1];
        lights[light].diffuse[2] = values[2];
    }  
    else if(pname==MGL_LIGHT_SPECULAR)
    {
        lights[light].specular[0] = values[0];
        lights[light].specular[1] = values[1];
        lights[light].specular[2] = values[2];
    }  
    else if(pname==MGL_LIGHT_POSITION)
    {
        // Transform points through the current modelview matrix
        MGLfloat v1[4] = {values[0], values[1], values[2], 1.0f};
        MGLfloat v2[4];
        mulMatrixVector(modelViewMatrix, v1, v2);
        lights[light].position[0] = v2[0]/v2[3];
        lights[light].position[1] = v2[1]/v2[3];
        lights[light].position[2] = v2[2]/v2[3];
        lights[light].position[3] = v2[3]/v2[3];
        
    }
    else {printf("[mglLight]: ERROR, unknown lighting property. Ignoring.\n");}
}

/**
 * Set the material properties for vertices that are being specified.
 * face specifies which face(s) the property changes are being applied
 * to, pname specifies the property being updated, and values is a
 * pointer to the float or floats which specify the value for that
 * parameter.  Parameters are handled as follows:
 *
 * MGL_MAT_AMBIENT
 * Set the ambient reflectance of the material.  values points to three
 * floats specifying the RGB (in that order) reflectance.  The initial
 * ambient reflectance of all materials should be (0.2, 0.2, 0.2).
 *
 * MGL_MAT_DIFFUSE
 * Set the diffuse reflectance of the material.  values points to three
 * floats specfiying the RGB (in that order) reflectance.  The initial
 * diffuse reflectance of all materials should be (0.8, 0.8, 0.8).
 *
 * MGL_MAT_SPECULAR
 * Set the specular reflectance of the material.  values points to three
 * floats specifying the RGB (in that order) reflectance.  The initial
 * specular reflectance for all materials should be (0.0, 0.0, 0.0).
 *
 * MGL_MAT_SHININESS
 * Set the specular exponent for the material.  values points to a single
 * float representing this exponent.  The initial specular exponent for
 * all materials should be 0.0.
 */
void mglMaterial(MGLmat_param pname,
				 MGLfloat *values)
{
    if(pname==MGL_MAT_AMBIENT)
    {
        materials.ambient[0] = values[0];
        materials.ambient[1] = values[1];
        materials.ambient[2] = values[2];
        
    }
    else if(pname == MGL_MAT_DIFFUSE)
    {
        materials.diffuse[0] = values[0];
        materials.diffuse[1] = values[1];
        materials.diffuse[2] = values[2];
    }
    else if(pname == MGL_MAT_SPECULAR)
    {
        materials.specular[0] = values[0];
        materials.specular[1] = values[1];
        materials.specular[2] = values[2];
    }
    else if(pname == MGL_MAT_SHININESS)
    {
        materials.shininess = values[0];
    }
    else {
        printf("[mglMaterial]: ERROR unknown property. Ignoring.\n");
    }
}

/**
 * Set whether textures should be drawn over surfaces which are being
 * specified.  Note that, unlike mglLightingEnabled(), this function
 * does not affect the render-time state of the scene - rather, it
 * affects the state of surfaces as they are specified.  Thus at render
 * time, we might have some surfaces with textures enabled, and some
 * without.  See also: mglTextureSlot.
 *
 * If lighting is disabled, then the specular texture slot is ignored,
 * and the diffuse texture is modulated with the color specified by 
 * mglColor.
 *
 * The initial value this attribute should be false.
 */
//void mglTexturesEnabled(bool enabled)
//{
//    if (hasBegun) MGL_ERROR("mglTranslate executed after mglBegin.");
//    
//    
//    if(currentTextureSlot==MGL_TEX_DIFFUSE)
//        diffuseTextureEnabled=enabled;
//    else if(currentTextureSlot==MGL_TEX_SPECULAR)
//        specularTextureEnabled=enabled;
//    
//}

/**
 * Load a texture into memory.  As long as textures have been enabled
 * with mglSetTexturesEnabled(), draw this texture over specified
 * surfaces until mglLoadTexture() is called again.  width and height
 * specify the dimensions of the image, and imageData points to an
 * array of RGB pixels.  The array begins with the bottom-left pixel
 * of the image, then the bottom-second-to-left, etc. - that is, elements
 * of the array are ordered first horizontally across the image, then
 * vertically up it.
 *
 * When this function is called, the in-use texture should be set
 * to the one that has been loaded - that is, the user doesn't need
 * to call both mglLoadTexture() and mglUseTexture() if she wishes
 * to use a newly loaded texture.
 *
 * Returns an ID (just an unsigned integer) by which the loaded
 * texture can be referenced in the future, e.g. with mglUseTexture().
 *
 * Note that you should make a copy of the data that's passed in,
 * rather than just store the imageData pointer.  The user can
 * use mglFreeTexture() to free the data that has been stored.
 */
//MGLtex_id mglLoadTexture(MGLsize width,
//						 MGLsize height,
//						 MGLpixel *imageData)
//{
//    if (hasBegun) MGL_ERROR("mglTranslate executed after mglBegin.");
//    
//    
//    printf("[mglLoadTexture]: width:%d height:%d\n", width, height);
//    MGLtexture * tex = new MGLtexture(width, height, imageData);
//    g_textures[tex->id] = tex;
//    
//    
//    if(currentTextureSlot==MGL_TEX_DIFFUSE)
//        currentDiffuseTextureID = tex->id;
//    else if(currentTextureSlot==MGL_TEX_SPECULAR)
//        currentSpecularTextureID = tex->id;
//	
//	
//    return tex->id;
//}

/**
 * Specify the texture which should be applied to newly specified
 * surfaces, using the ID returned by mglLoadTexture().  When a 
 * texture is applied to the surface, it should be modulated 
 * (multiplied) with the material color.  See also: mglTextureSlot.
 * 
 * By default, the texture ID is set to zero (the NULL texture).
 */
//void mglUseTexture(MGLtex_id id)
//{
//    if (hasBegun) MGL_ERROR("mglTranslate executed after mglBegin.");
//    
//    MGLtexture * tex = g_textures[id];
//    
//    if(tex==NULL)
//    {
//        printf("[mglUseTexture]: ERROR, texture is not valid!\n");
//        return;
//    }
//    
//    if(currentTextureSlot==MGL_TEX_DIFFUSE)
//        currentDiffuseTextureID = id;
//    else if(currentTextureSlot==MGL_TEX_SPECULAR)
//        currentSpecularTextureID = id;
//}

/**
 * Free ay_n memory associated with the specified texture.  A user
 * should call this once she knows a texture will no longer be used.
 * Note that this invalidates the given ID - that is, an error
 * should be thrown if it is later passed to mglUseTexture().
 */
//void mglFreeTexture(MGLtex_id id)
//{
//    if (hasBegun) MGL_ERROR("mglTranslate executed after mglBegin.");
//    
//    MGLtexture * tex = g_textures[id];
//    
//    if(tex!=NULL)
//    {
//        delete tex;
//        tex = NULL;
//        g_textures.erase (id);   
//    }
//}

/**
 * Should be called just before a call to mglVertex*(); specifies
 * a texture coordinate on the current texture for the vertex
 * which is about to be specified.
 */
//void mglTexCoord(MGLfloat x,
//				 MGLfloat y)
//{
//    currentU = x;
//    currentV = y;
//}

/**
 * Sets the active texture slot.  There are two texture slots.
 * 
 * If MGL_TEX_DIFFUSE is set, then calls to mglTexturesEnabled
 * and mglUseTexture enable texturing and set the texture 
 * for the diffuse color of the object, respectively.
 *
 * If MGL_TEX_SPECULAR is set, then calls to mglTexturesEnabled
 * and mglUseTexture enable texturing and set the texture
 * for the specular color of the object, respectively.
 *
 * The initial value for this attribute should be MGL_TEX_DIFFUSE.
 */
//void mglTextureSlot(MGLtex_slot slot)
//{
//    if (hasBegun) MGL_ERROR("mglTranslate executed after mglBegin.");
//    
//    
//    if(slot==MGL_TEX_DIFFUSE)
//        currentTextureSlot = MGL_TEX_DIFFUSE;
//    else if(slot == MGL_TEX_SPECULAR)
//        currentTextureSlot = MGL_TEX_SPECULAR;
//    else {
//        printf("[mglTextureSlot]: ERROR, unknown texture slot. Ignoring\n");
//    }
//	
//}

/**
 * Specify the normal vector for subsequent vertices, to be
 * used in lighting computations.  Note that this normal is
 * persistent - it should be used for all specified vertices until
 * this function has been called again.
 */
void mglNormal(MGLfloat x,
			   MGLfloat y,
			   MGLfloat z)
{
    currentNormal[0] = x;
    currentNormal[1] = y;
    currentNormal[2] = z; 
}

/**
 * Force execution of minigl commands in finite time.
 *
 * We will use this command differently from what it should be
 * originally. It is always called after mglReadPixels, being
 * the last one among the sequence of minigl calls. So this
 * function is a place where you put all the cleanup codes and
 * reset the states such as global variables.
 */
void mglFlush()
{
    if (hasBegun) MGL_ERROR("mglFlush executed after mglBegin.");
	
    transformedVertices.clear();
}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
    if (hasBegun) MGL_ERROR("mglBegin executed after mglBegin.");
	
    hasBegun = true;
    curPolyMode = mode;
}

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd()
{
    if (!hasBegun) MGL_ERROR("mglEnd has unmatched mglBegin.");
	
    hasBegun = false;
    if (curIndex != 0) MGL_ERROR("incompleted primitives specified.");
}

/**
 * Specify a two-dimensional vertex; the x- and y-coordinates
 * are explicitly specified, while the z-coordinate is assumed
 * to be zero.  Must appear between calls to mglBegin() and
 * mglEnd().
 */
void mglVertex2(MGLfloat x,
                MGLfloat y)
{
    mglVertex3(x, y, 0.0f);
}

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x,
                MGLfloat y,
                MGLfloat z)
{
    if (!hasBegun) MGL_ERROR("mglVertex executed outside mglBegin/mglEnd.");
	
    MGLfloat v1[4] = {x, y, z, 1.0f};
    MGLfloat v2[4];
    mulMatrixVector(modelViewMatrix, v1, v2);
    mulMatrixVector(projectionMatrix, v2, v1);
	
    MGLvertex v;
    v.x = v1[0];
    v.y = v1[1];
    v.z = v1[2];
    v.w = v1[3];
	
    ///////////////////////////////////////////////////////
    //
    // TODO:
    //
    // Assign your new added vertex attributes here.
    //
    // ////////////////////////////////////////////////////
    
    // Normals
	MGLfloat n1[4] = {currentNormal[0], currentNormal[1], currentNormal[2], 0.0};
	MGLfloat n2[4] = {0.0, 0.0, 0.0, 0.0};
	mulMatrixVector(curNormalMatrix, n1, n2);
	
	// normalize
	norm(n2);
	v.x_n = n2[0]; v.y_n = n2[1]; v.z_n = n2[2];
	
    // Eye TODO: do these go through the projection transformation???
    
    v.x_eye = v2[0]/v2[3];
    v.y_eye = v2[1]/v2[3];
    v.z_eye = v2[2]/v2[3];
    
    v.u = currentU;
    v.v = currentV;
    
	
    v.color = curColor;
	
    ///////////////////////////////////////////////////////
    //
    // END HERE
    //
    ///////////////////////////////////////////////////////
	
    transformedVertices.push_back(v);
	
    if (curPolyMode == MGL_TRIANGLES) curIndex = (curIndex + 1) % 3;
    else if (++curIndex == 4) {
        transformedVertices.push_back(*(transformedVertices.end() - 4));
        transformedVertices.push_back(*(transformedVertices.end() - 3));
        curIndex = 0;
    }
}

// Inverse of a 3 x 3 matrix
void inverse33(MGLmatrix & M) {
    
    float *m = M.m;
    MGLmatrix R;
    float d =  m[0] * m[5] *m[10] - m[0] *m[9]* m[6] - m[4] *m[1]* m[10] + m[4] *m[9] *m[2] +  m[8] *m[1] *m[6] - m[8] *m[5] *m[2];
    R.m[0] = (m[5]*m[10] - m[9]*m[6])/d;
    R.m[1] = -(m[1]*m[10] - m[9]*m[2])/d;
    R.m[2] = (m[1]*m[6] - m[5]*m[2])/d;
    R.m[3] = 0;
    R.m[4] = -(m[4]*m[10] - m[8]*m[6])/d;
    R.m[5] = (m[0]*m[10] - m[8]*m[2])/d;
    R.m[6] = -(m[0]*m[6] - m[4]*m[2])/d;
    R.m[7] = 0;
    R.m[8] =  (m[4]*m[9] - m[8]*m[5])/d;
    R.m[9] = -(m[0]*m[9] - m[8]*m[1])/d;
    R.m[10] = (m[0]*m[5] - m[4]*m[1])/d;
    R.m[11] = 0;
    R.m[12] = R.m[13] = R.m[14] = R.m[15] = 0;
    
    M = R;
}

void computeNormalMatrix()
{
    // TODO: make sure this works in all cases
    MGLmatrix M = modelViewMatrix;
	MGLmatrix output;
	bool didWork = mglInvertMatrix(M.m, output.m);
	
	if(!didWork)
	{
		printf("[computeNormalMatrix]: ERROR, problem compute the normal matrix\n");
		return;
	}
					
    //inverse33(M);
    tMatrix(M.m); 
    curNormalMatrix = M;
}



/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{
    if (hasBegun) MGL_ERROR("mglMatrixMode executed after mglBegin.");
	
    curMatrixMode = mode;
    if (mode == MGL_MODELVIEW) curMatrix = &modelViewMatrix;
    else curMatrix = &projectionMatrix;
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{
    if (hasBegun) MGL_ERROR("mglPushMatrix executed after mglBegin.");
	
    if (curMatrixMode == MGL_MODELVIEW) 
    {
        modelViewStack.push(modelViewMatrix);
        //computeNormalMatrix(); // TODO: double check you don't need this one
    }
    else 
        projectionStack.push(projectionMatrix);
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
    if (hasBegun) MGL_ERROR("mglPopMatrix executed after mglBegin.");
	
    if (curMatrixMode == MGL_MODELVIEW) {
        if (modelViewStack.empty()) MGL_ERROR("mglPopMatrix underflow the stack.");
		
        modelViewMatrix = modelViewStack.top();
        modelViewStack.pop();
        
        computeNormalMatrix();
        
        
    } else {
        if (projectionStack.empty()) MGL_ERROR("mglPopMatrix underflow the stack.");
		
        projectionMatrix = projectionStack.top();
        projectionStack.pop();
		
    }
}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
    if (hasBegun) MGL_ERROR("mglLoadIdentity executed after mglBegin.");
	
    for (int i = 0; i < 16; ++i) {
        curMatrix->m[i] = (i % 5 == 0);
    }
    
    if (curMatrixMode == MGL_MODELVIEW) 
    {
        computeNormalMatrix();
    }
}

/**
 * Replace the current matrix with an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglLoadMatrix(const MGLfloat *matrix)
{
    if (hasBegun) MGL_ERROR("mglLoadMatrix executed after mglBegin.");
	
    memcpy(curMatrix->m, matrix, 16 * sizeof(MGLfloat));
    
    computeNormalMatrix();
}

void multMatrix(const MGLfloat * right, const MGLfloat * left, MGLmatrix * output)
{
    MGLmatrix result;
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            MGLfloat sum = 0;
            for (int k = 0; k < 4; ++k) {
                sum += right[k * 4 + j] * left[i * 4 + k];
            }
            result.m[i * 4 + j] = sum;
        }
    }	
    *output = result;
}


/**
 * Multiply the current matrix by an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglMultMatrix(const MGLfloat *matrix)
{
    if (hasBegun) MGL_ERROR("mglMultMatrix executed after mglBegin.");

    multMatrix(curMatrix->m, matrix, curMatrix);
    computeNormalMatrix();
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x,
                  MGLfloat y,
                  MGLfloat z)
{
    if (hasBegun) MGL_ERROR("mglTranslate executed after mglBegin.");
	
    MGLmatrix tMat;
    tMat.m[12] = x;
    tMat.m[13] = y;
    tMat.m[14] = z;
 
    multMatrix(curMatrix->m, tMat.m, curMatrix);
 
    computeNormalMatrix();
}

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle,
               MGLfloat x,
               MGLfloat y,
               MGLfloat z)
{
    if (hasBegun) MGL_ERROR("mglRotate executed after mglBegin.");
	
    MGLmatrix rMat;
    angle *= M_PI / 180.0f;
    // normalize vector <x, y, z>
    MGLfloat l = sqrt(x * x + y * y + z * z);
    if (l == 0.0f) MGL_ERROR("mglRotate vector must be non-zero.");
    x /= l;
    y /= l;
    z /= l;
    MGLfloat c = cos(angle);
    MGLfloat s = sin(angle);
	
    rMat.m[0] = x * x * (1.0f - c) + c;
    rMat.m[1] = y * x * (1.0f - c) + z * s;
    rMat.m[2] = x * z * (1.0f - c) - y * s;
    rMat.m[4] = x * y * (1.0f - c) - z * s;
    rMat.m[5] = y * y * (1.0f - c) + c;
    rMat.m[6] = y * z * (1.0f - c) + x * s;
    rMat.m[8] = x * z * (1.0f - c) + y * s;
    rMat.m[9] = y * z * (1.0f - c) - x * s;
    rMat.m[10] = z * z * (1.0f - c) + c;

    multMatrix(curMatrix->m, rMat.m, curMatrix);
    
	// compute the normal matrix
    computeNormalMatrix();
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z)
{
    if (hasBegun) MGL_ERROR("mglScale executed after mglBegin.");
	
    MGLmatrix sMat;
    sMat.m[0] = x;
    sMat.m[5] = y;
    sMat.m[10] = z;

    multMatrix(curMatrix->m, sMat.m, curMatrix);
    
	// compute the normal matrix
    computeNormalMatrix();
	
}

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left,
                MGLfloat right,
                MGLfloat bottom,
                MGLfloat top,
                MGLfloat near,
                MGLfloat far)
{
    if (hasBegun) MGL_ERROR("mglFrustum executed after mglBegin.");
    if (left == right || bottom == top || near == far ||
        near <= 0 || far <= 0) {
        MGL_ERROR("mglFrustum invalid arguments.");
    }
	
    MGLmatrix fMat;
    fMat.m[0] = 2.0f * near / (right - left);
    fMat.m[5] = 2.0f * near / (top - bottom);
    fMat.m[8] = (right + left) / (right - left);
    fMat.m[9] = (top + bottom) / (top - bottom);
    fMat.m[10] = -(far + near) / (far - near);
    fMat.m[11] = -1.0f;
    fMat.m[14] = -2.0f * far * near / (far - near);
    fMat.m[15] = 0.0f;
    mglMultMatrix(fMat.m);
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left,
              MGLfloat right,
              MGLfloat bottom,
              MGLfloat top,
              MGLfloat near,
              MGLfloat far)
{
    if (hasBegun) MGL_ERROR("mglOrtho executed after mglBegin.");
    if (left == right || bottom == top || near == far) {
        MGL_ERROR("mglOrtho invalid arguments.");
    }
	
    MGLmatrix oMat;
    oMat.m[0] = 2.0f / (right - left);
    oMat.m[5] = 2.0f / (top - bottom);
    oMat.m[10] = -2.0f / (far - near);
    oMat.m[12] = -(right + left) / (right - left);
    oMat.m[13] = -(top + bottom) / (top - bottom);
    oMat.m[14] = -(far + near) / (far - near);
    mglMultMatrix(oMat.m);
}


void mglPerspective(float fovyInDegrees, float aspectRatio,
                      float znear, float zfar)
{
	mglMatrixMode( MGL_PROJECTION );
	
    float ymax, xmax;
    //float temp, temp2, temp3, temp4;
    ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
    //ymin = -ymax;
    //xmin = -ymax * aspectRatio;
    xmax = ymax * aspectRatio;
    mglFrustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

 
//-----------------------------------------------------------------------------
// name: lookAt()
// desc: set eye, at, up vector
//       (from jshmrsn, macrumors forum)
//-----------------------------------------------------------------------------
void mglLookAt( MGLfloat eye_x, MGLfloat eye_y, MGLfloat eye_z,
				   MGLfloat at_x, MGLfloat at_y, MGLfloat at_z,
				   MGLfloat up_x, MGLfloat up_y, MGLfloat up_z )
{
    MGLfloat m[16];
    MGLfloat x[3], y[3], z[3];
    MGLfloat mag;
	
    /* Make rotation matrix */
	
    /* Z vector */
    z[0] = eye_x - at_x;
    z[1] = eye_y - at_y;
    z[2] = eye_z - at_z;
	
    mag = sqrt( z[0] * z[0] + z[1] * z[1] + z[2] * z[2] );
    if( mag )
    {   /* mpichler, 19950515 */
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }
	
    /* Y vector */
    y[0] = up_x;
    y[1] = up_y;
    y[2] = up_z;
	
    /* X vector = Y cross Z */
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];
	
    /* Recompute Y = Z cross X */
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];
	
    /* mpichler, 19950515 */
    /* cross product gives area of parallelogram, which is < 1.0 for
     * non-perpendicular unit-length vectors; so normalize x, y here
     */
    mag = sqrt( x[0] * x[0] + x[1] * x[1] + x[2] * x[2] );
    if( mag )
    {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }
    mag = sqrt( y[0] * y[0] + y[1] * y[1] + y[2] * y[2] );
    if( mag )
    {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }
	
#define M(row,col)  m[col*4+row]
    M(0,0) = x[0];
    M(0,1) = x[1];
    M(0,2) = x[2];
    M(0,3) = 0.0;
    M(1,0) = y[0];
    M(1,1) = y[1];
    M(1,2) = y[2];
    M(1,3) = 0.0;
    M(2,0) = z[0];
    M(2,1) = z[1];
    M(2,2) = z[2];
    M(2,3) = 0.0;
    M(3,0) = 0.0;
    M(3,1) = 0.0;
    M(3,2) = 0.0;
    M(3,3) = 1.0;    
#undef M
	
	mglMatrixMode(MGL_MODELVIEW);
	//mglLoadIdentity();// TODO: should this be here?
	
    // multiply into m
    mglMultMatrix( m );
	
    // translate eye to origin
    mglTranslate( -eye_x, -eye_y, -eye_z );
}




/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
    MGL_SET_RED(curColor, red);
    MGL_SET_GREEN(curColor, green);
    MGL_SET_BLUE(curColor, blue);
}




MGLfloat * mglGetModelViewMatrixPtr( )
{
	return modelViewMatrix.m;
	
}

MGLfloat * mglGetProjectionMatrixPtr( )
{
	return projectionMatrix.m;
	
}

MGLfloat * mglGetNormalMatrixPtr( )
{
	return modelViewMatrix.m;
	
}


MGLfloat * mglGetLightParameterPtr(MGLlight light, MGLlight_param param)
{
	
	if( (light < 0) || (light > 2))
	{
		printf("[mglGetLightParameterPtr]: ERROR, invalid light number\n");
		return NULL;
	}
	
	if(param == MGL_LIGHT_POSITION)
		return lights[light].position;
	else if(param == MGL_LIGHT_AMBIENT)
		return lights[light].ambient;
	else if(param == MGL_LIGHT_DIFFUSE)
		return lights[light].diffuse;
	else if(param == MGL_LIGHT_SPECULAR)
		return lights[light].specular;
	else {
		printf("[mglGetLightParameterPtr]: ERROR, invalid parameter\n");
		return NULL;
	}
}

////////// TODO ////////////////////


// TAKE FROM http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
bool mglInvertMatrix(const float * m, float * invOut)
{
	float inv[16], det;
	int i;
	
	inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
	+ m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
	- m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
	+ m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
	- m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
	- m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
	+ m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
	- m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
	+ m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
	+ m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
	- m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
	+ m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
	- m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
	- m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
	+ m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
	- m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
	+ m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];
	
	det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	if (det == 0)
        return false;
	
	det = 1.0 / det;
	
	for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;
	
	return true;
}

