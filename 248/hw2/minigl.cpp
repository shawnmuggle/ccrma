/****************************************************************************
 * CS248 Assignment 2, Winter 2012
 *
 * Filename: minigl.cpp
 * Authors: Ming Jiang (ming.jiang@stanford.edu), Alexis Haraux (aharaux@stanford.edu)
 *
 * Description: MiniGL implementation starter code for assignment 2.
 ****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>
#include <vector>
#include <stack>
#include <map>
#include "minigl.h"

using namespace std;


// Macros and constants definition

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define NOT_YET_IMPLEMENTED printf("Warning: call to unimplimented function!\n")

/**
 * A utility function for error reporting.
 */
inline void MGL_ERROR(const char* description) {
    printf("ERROR: %s\n", description);
    exit(1);
}


// Data structures definition

struct Light
{
    bool enabled;
    std::map<MGLlight_param, std::vector<MGLfloat> > params;
};

struct Material
{
    std::map<MGLmat_param, std::vector<MGLfloat> > params;
};

struct Texture
{
    Texture() :
    width(0), height(0), data(NULL)
    { }
    
    Texture(MGLsize width, MGLsize height, MGLpixel *imageData) : 
    width(width), height(height)
    {
        data = (MGLpixel *)malloc(width * height * sizeof(MGLpixel));
        for (int i = 0; i < width * height; i++)
        {
            data[i] = imageData[i];
        }
    }
    
    MGLpixel getColor(MGLfloat u, MGLfloat v)
    {
        if (!data)
        {
            return 0xffffffff;
        }
        
        while (u < 0.0f)
        {
            u += 1.0f;
        }
        while (v < 0.0f)
        {
            v += 1.0f;
        }
        u = fmodf(u, 1.0f);
        v = fmodf(v, 1.0f);
        int x0 = floor(u * width);
        int x1 = fmodf(ceil(u * width), width);
        int y0 = floor(v * height);
        int y1 = fmodf(ceil(v * height), height);
        MGLpixel x0y0 = data[y0 * width + x0];
        MGLbyte x0y0r = MGL_GET_RED(x0y0);
        MGLbyte x0y0g = MGL_GET_GREEN(x0y0);
        MGLbyte x0y0b = MGL_GET_BLUE(x0y0);

        MGLpixel x0y1 = data[y1 * width + x0];
        MGLbyte x0y1r = MGL_GET_RED(x0y1);
        MGLbyte x0y1g = MGL_GET_GREEN(x0y1);
        MGLbyte x0y1b = MGL_GET_BLUE(x0y1);

        MGLpixel x1y0 = data[y0 * width + x1];
        MGLbyte x1y0r = MGL_GET_RED(x1y0);
        MGLbyte x1y0g = MGL_GET_GREEN(x1y0);
        MGLbyte x1y0b = MGL_GET_BLUE(x1y0);

        MGLpixel x1y1 = data[y1 * width + x1];
        MGLbyte x1y1r = MGL_GET_RED(x1y1);
        MGLbyte x1y1g = MGL_GET_GREEN(x1y1);
        MGLbyte x1y1b = MGL_GET_BLUE(x1y1);
        
        MGLfloat s = v * height - y0;
        MGLfloat t = u * width - x0;
        
        MGLbyte r = (1.0f - s) * ((1.0f - t) * x0y0r + t * x1y0r) + s * ((1.0f - t) * x0y1r + t * x1y1r);
        MGLbyte g = (1.0f - s) * ((1.0f - t) * x0y0g + t * x1y0g) + s * ((1.0f - t) * x0y1g + t * x1y1g);
        MGLbyte b = (1.0f - s) * ((1.0f - t) * x0y0b + t * x1y0b) + s * ((1.0f - t) * x0y1b + t * x1y1b);
        
        MGLpixel p = 0xffffffff;
        MGL_SET_RED(p, r);
        MGL_SET_GREEN(p, g);
        MGL_SET_BLUE(p, b);
        
        return p;
    }
    
    MGLsize width;
    MGLsize height;
    MGLpixel *data;
};

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
    
    // Texture coordinates
    MGLfloat u, v;
    MGLfloat f_prime_u, f_prime_v, g_prime;
    Texture diffuseTexture;
    Texture specularTexture;
    
    // Normal coordinates
    MGLfloat nx, ny, nz;
    MGLfloat f_prime_nx, f_prime_ny, f_prime_nz;
    
    // Eye-space coordinates
    MGLfloat ex, ey, ez;
    MGLfloat f_prime_ex, f_prime_ey, f_prime_ez;
    
    Material material;
    
    MGLvertex()
    {
        color = 0xff00000;
    }
    
    ///////////////////////////////////////////////////////
    //
    // END HERE
    //
    ///////////////////////////////////////////////////////
};

struct MGLtriangle {
    MGLvertex v[3];
};

/**
 * A simple wrapper structure for storing 4x4 matrix.
 * Constructed to be identity matrix initially.
 */
struct MGLmatrix {
    MGLmatrix() {
        memset(m, 0, 16 * sizeof(MGLfloat));
        m[0] = m[5] = m[10] = m[15] = 1;
    }
    MGLfloat m[16];
};

/**
 * A structure for you to contain all information about a fragment.
 * Currently it only contains color and depth.
 *
 **********************************************************
 *
 * TODO: you will need to add new attributes here.
 *
 **********************************************************
 */
struct MGLfragment {
    MGLfragment() {
        color = 0xff000000;
        depth = 1.0f;
    }
    MGLpixel color;
    MGLfloat depth;
};

/**
 * A structure (actually a class) abstracted for a fragment buffer.
 * This is the place where your rasterizer will output.  Final color
 * for the frame buffer will be calculated based on this fragment
 * buffer.
 */
struct MGLfragbuffer {
    MGLfragbuffer(MGLsize w, MGLsize h) : width(w), height(h) {
        data = new MGLfragment[w * h];
    }
    ~MGLfragbuffer() {
        delete [] data;
    }
    
    MGLsize width, height;
    MGLfragment* data;
};

// Global variables definition

MGLpixel curColor = 0xff000000;                // changed when mglColor
// is called
MGLmatrix_mode curMatrixMode = MGL_MODELVIEW;  // current matrix mode,
// changed when mglMatrixMode
// is called
MGLpoly_mode curPolyMode = MGL_TRIANGLES;      // current polygon mode
// specified by mglBegin

MGLbool hasBegun = false;  // true when we enter mglBegin and
// false when we leave mglEnd
MGLint curIndex = 0;       // used to ensure correct number of
// calls to mglVertex

MGLmatrix modelViewMatrix, projectionMatrix;  // store the current modelview
// and projection matrices
MGLmatrix* curMatrix;                         // always point to one of the
// above matrix according to the
// current matrix mode
stack<MGLmatrix> modelViewStack;              // modelview matrix stack
stack<MGLmatrix> projectionStack;             // projection matrix stack

vector<MGLvertex> transformedVertices;  // store all vertices in clipping space
// coordinates without dividing by w

bool lightingEnabled = false;
std::map<MGLlight, Light> lights;
MGLshading_mode shadingMode = MGL_PHONG;

bool texturesEnabled = false;
std::map<MGLtex_id, Texture> textures;
MGLtex_id nextTextureId = 1;
MGLtex_slot textureSlot = MGL_TEX_DIFFUSE;
MGLtex_id currentDiffuseTextureId = 0;
MGLtex_id currentSpecularTextureId = 0;
struct texCoord {
    MGLfloat u, v;
};
texCoord currentTexCoord;

class Vector3 {
public:
    Vector3()
    {
    }
    Vector3(MGLfloat vin[3])
    {
        v[0] = vin[0];
        v[1] = vin[1];
        v[2] = vin[2];
    }
    Vector3(MGLfloat x, MGLfloat y, MGLfloat z)
    {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }
    Vector3 operator-(Vector3 rhs)
    {
        return Vector3(v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2]);
    }
    Vector3 operator*(MGLfloat rhs)
    {
        return Vector3(v[0] * rhs, v[1] * rhs, v[2] * rhs);
    }
    Vector3 operator/(MGLfloat rhs)
    {
        return *this * (1.0 / rhs);
    }
    MGLfloat magnitude()
    {
        return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
    }
    MGLfloat dot(Vector3 rhs)
    {
        return v[0] * rhs.v[0] + v[1] * rhs.v[1] + v[2] * rhs.v[2];
    }
    Vector3 cross(Vector3 rhs)
    {
        return Vector3(v[1] * rhs.v[2] - v[2] * rhs.v[1], 
                       v[2] * rhs.v[0] - v[0] * rhs.v[2], 
                       v[0] * rhs.v[1] - v[1] * rhs.v[0]);
    }
    Vector3 normalize()
    {
        return *this / magnitude();
    }
    MGLfloat cosAngleToVector(Vector3 rhs)
    {
        return dot(rhs) / (magnitude() * rhs.magnitude());
        
    }
    MGLfloat v[3];
};

class Matrix3;
Matrix3 matrix3FromRows(vector<Vector3 > rows);
class Matrix3
{
public:
    // row major, top to bottom
    vector<Vector3> columns;
    Matrix3() { }
    Matrix3(vector<Vector3> columns) : columns(columns) { }
    Matrix3 operator*(MGLfloat rhs)
    {
        Matrix3 ret;
        for (int i = 0; i < 3; i++)
        {
            ret.columns.push_back(Vector3());
            ret.columns[i].v[0] = columns[i].v[0] * rhs;
            ret.columns[i].v[1] = columns[i].v[1] * rhs;
            ret.columns[i].v[2] = columns[i].v[2] * rhs;
        }
        return ret;
    }
    Matrix3 operator/(MGLfloat rhs)
    {
        Matrix3 lhs = *this;
        return lhs * (1.0 / rhs);
    }
    Vector3 operator*(Vector3 rhs)
    {
        Vector3 ret;
        for (int i = 0; i < 3; i++)
        {
            ret.v[i] = columns[0].v[i] * rhs.v[0] + columns[1].v[i] * rhs.v[1] + columns[2].v[i] * rhs.v[2];
        }
        return ret;
    }
    Matrix3 transpose()
    {
        return matrix3FromRows(columns);
    }
    Matrix3 invert()
    {
        Matrix3 inversion;
        inversion.columns.push_back(columns[1].cross(columns[2]));
        inversion.columns.push_back(columns[2].cross(columns[0]));
        inversion.columns.push_back(columns[0].cross(columns[1]));
        MGLfloat det = columns[0].dot(columns[1].cross(columns[2]));
        return inversion.transpose() / det;
    }
};

Matrix3 matrix3FromRows(vector<Vector3 > rows)
{
    Matrix3 newMatrix;

    for (int i = 0; i < 3; i++)
    {
        Vector3 col;
        col.v[0] = rows[0].v[i];
        col.v[1] = rows[1].v[i];
        col.v[2] = rows[2].v[i];
        newMatrix.columns.push_back(col);
    }
    
    return newMatrix;
}
Matrix3 matrix3FromMGLMatrix(MGLmatrix const & mat)
{
    Matrix3 newMatrix;
    for (int i = 0; i < 3; i++)
    {
        newMatrix.columns.push_back(Vector3());
        for (int j = 0; j < 3; j++)
        {
            newMatrix.columns[i].v[j] = mat.m[i * 4 + j];
        }

    }
    return newMatrix;
}

Vector3 currentNormal;
Material currentMaterial;

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


/**
 * Rasterizing a triangle.
 * You will have to update this function to handle 
 * perspective correct interpolation of fragment 
 * attributes (color, texture coordinates, normals,
 * eye space coordinates)
 *
 */
void rasterize(MGLtriangle& tri, MGLfragbuffer& fragbuf)
{
    // Viewport transformation
    for (int i = 0; i < 3; ++i) {
        tri.v[i].x = 0.5f * fragbuf.width * (tri.v[i].x / tri.v[i].w + 1.0f);
        tri.v[i].y = 0.5f * fragbuf.height * (tri.v[i].y / tri.v[i].w + 1.0f);
        tri.v[i].z = 0.5f * (tri.v[i].z / tri.v[i].w + 1.0f);
        
        ///////////////////////////////////////////////////
        //
        // TODO:
        //
        // You may want to keep the original w value for
        // perspective correction later.  Also you may want
        // to do some transformation to other attributes
        // you added to MGLvertex as well.
        //
        ///////////////////////////////////////////////////
        
        
        
        ///////////////////////////////////////////////////
        //
        // END HERE
        //
        ///////////////////////////////////////////////////
    }
    
	
	// Compute the bounding rectangle
	float xll = fmax(0.0f, fmin(tri.v[0].x, fmin(tri.v[1].x, tri.v[2].x)));
	float yll = fmax(0.0f, fmin(tri.v[0].y, fmin(tri.v[1].y, tri.v[2].y)));
	float xur = fmin((float)(fragbuf.width-1), fmax(tri.v[0].x, fmax(tri.v[1].x, tri.v[2].x)));
	float yur = fmin((float)(fragbuf.height-1), fmax(tri.v[0].y, fmax(tri.v[1].y, tri.v[2].y)));
	
	// Compute the common denominator
	float detA = (tri.v[0].x * tri.v[1].y) - (tri.v[0].x * tri.v[2].y) - (tri.v[1].x * tri.v[0].y) 
	+ (tri.v[1].x * tri.v[2].y) + (tri.v[2].x * tri.v[0].y) - (tri.v[2].x * tri.v[1].y);
	
	// Draw the triangle
	
	for(int y = floor(yll); y <= ceil(yur); y+=1) {	
		for(int x = floor(xll); x <= ceil(xur); x+=1) { 
			
			MGLvertex p;
			
			// Use the pixel center
			p.x = x+.5;
			p.y = y+.5;
			
			// Compute barycentric coordinates
			float alpha = (p.x * tri.v[1].y) - (p.x * tri.v[2].y) - (tri.v[1].x * p.y) + (tri.v[1].x * tri.v[2].y) + (tri.v[2].x * p.y) - (tri.v[2].x * tri.v[1].y);
			alpha /= detA;
			float beta = (tri.v[0].x * p.y) - (tri.v[0].x * tri.v[2].y) - (p.x * tri.v[0].y) + (p.x * tri.v[2].y) + (tri.v[2].x * tri.v[0].y) - (tri.v[2].x * p.y);
			beta /= detA;
//			float gamma = (tri.v[0].x * tri.v[1].y) - (tri.v[0].x * p.y) - (tri.v[1].x * tri.v[0].y) + (tri.v[1].x * p.y) + (p.x * tri.v[0].y) - (p.x * tri.v[1].y);
//			gamma /= detA;
            float gamma = 1.0 - alpha - beta;
			
			// Point/Triangle test
			if (alpha >= 0. && alpha <= 1. && beta >= 0. && beta <= 1. && gamma >= 0. && gamma <=1.)
			{
				///////////////////////////////////////////////////
				//
				// TODO:
				//
				// Here you will have to interpolate any useful
				// fragment attribute with barycentric interpolation.
				// Be careful that some attributes need perspective
				// correction (unlike z). 
				// You will then save those attributes in the fragment
				// buffer.
				//
				///////////////////////////////////////////////////
				
				
				// Barycentric interpolation of z
				p.z = alpha * tri.v[0].z + beta * tri.v[1].z + gamma * tri.v[2].z;
                
                // Arbitrarily choose the texture from v0, because it's undefined behavior to have different textures for the different vertices in a triangle.
                Texture diffuseTexture = tri.v[0].diffuseTexture;
                Texture specularTexture = tri.v[0].specularTexture;
                                
                MGLfloat linearInvW = alpha * tri.v[0].g_prime + beta * tri.v[1].g_prime + gamma * tri.v[2].g_prime;

                // Barycentric interpolation of u & z
                MGLfloat linearU = alpha * tri.v[0].f_prime_u + beta * tri.v[1].f_prime_u + gamma * tri.v[2].f_prime_u;
                MGLfloat linearV = alpha * tri.v[0].f_prime_v + beta * tri.v[1].f_prime_v + gamma * tri.v[2].f_prime_v;
                
                p.u = linearU / linearInvW;
                p.v = linearV / linearInvW;
                
                MGLfloat linearNx = alpha * tri.v[0].f_prime_nx + beta * tri.v[1].f_prime_nx + gamma * tri.v[2].f_prime_nx;
                MGLfloat linearNy = alpha * tri.v[0].f_prime_ny + beta * tri.v[1].f_prime_ny + gamma * tri.v[2].f_prime_ny;
                MGLfloat linearNz = alpha * tri.v[0].f_prime_nz + beta * tri.v[1].f_prime_nz + gamma * tri.v[2].f_prime_nz;
                
                p.nx = linearNx / linearInvW;
                p.ny = linearNy / linearInvW;
                p.nz = linearNz / linearInvW;
                
                MGLpixel diffuseColor = diffuseTexture.getColor(p.u, p.v);
                MGLpixel specularColor = specularTexture.getColor(p.u, p.v);
                
                MGLfloat linearEx = alpha * tri.v[0].f_prime_ex + beta * tri.v[1].f_prime_ex + gamma * tri.v[2].f_prime_ex;
                MGLfloat linearEy = alpha * tri.v[0].f_prime_ey + beta * tri.v[1].f_prime_ey + gamma * tri.v[2].f_prime_ey;
                MGLfloat linearEz = alpha * tri.v[0].f_prime_ez + beta * tri.v[1].f_prime_ez + gamma * tri.v[2].f_prime_ez;
                
                p.ex = linearEx / linearInvW;
                p.ey = linearEy / linearInvW;
                p.ez = linearEz / linearInvW;
                
                MGLfloat r = 0;
                MGLfloat g = 0;
                MGLfloat b = 0;
                if (lightingEnabled)
                {
                    MGLfloat r_amb, g_amb, b_amb;
                    MGLfloat r_dif, g_dif, b_dif;
                    MGLfloat r_spc, g_spc, b_spc;
                    
                    Material m = tri.v[0].material;
                    for (std::map<MGLlight, Light>::iterator i = lights.begin(); i != lights.end(); i++)
                    {
                        Light l = (*i).second;
                        if (l.enabled)
                        {
                            r_amb = fmin(1.0, l.params[MGL_LIGHT_AMBIENT][0] * m.params[MGL_MAT_AMBIENT][0]);
                            g_amb = fmin(1.0, l.params[MGL_LIGHT_AMBIENT][1] * m.params[MGL_MAT_AMBIENT][1]);
                            b_amb = fmin(1.0, l.params[MGL_LIGHT_AMBIENT][2] * m.params[MGL_MAT_AMBIENT][2]);
                            
                            Vector3 pointPosition(p.ex, p.ey, p.ez);
                            vector<MGLfloat> lightPos = l.params[MGL_LIGHT_POSITION];
                            
                            Vector3 lightPosition(lightPos[0], lightPos[1], lightPos[2]);
                            Vector3 pointToLight = (pointPosition - lightPosition).normalize();
                            Vector3 normal = Vector3(p.nx, p.ny, p.nz).normalize();
                            MGLfloat cosTheta = fmax(0.0, normal.dot(pointToLight));
                            
                            r_dif = fmin(1.0, cosTheta * m.params[MGL_MAT_DIFFUSE][0] * l.params[MGL_LIGHT_DIFFUSE][0]);
                            g_dif = fmin(1.0, cosTheta * m.params[MGL_MAT_DIFFUSE][1] * l.params[MGL_LIGHT_DIFFUSE][1]);
                            b_dif = fmin(1.0, cosTheta * m.params[MGL_MAT_DIFFUSE][2] * l.params[MGL_LIGHT_DIFFUSE][2]);
                            
                            Vector3 reflection = (normal * 2 * pointToLight.dot(normal) - pointToLight).normalize();
                            Vector3 pointToCamera = (pointPosition - Vector3(0, 0, 0)).normalize();
                            cosTheta = powf(fmax(0.0, pointToCamera.dot(reflection)), m.params[MGL_MAT_SHININESS][0]);
                            
                            r_spc = fmin(1.0, cosTheta * m.params[MGL_MAT_SPECULAR][0] * l.params[MGL_LIGHT_SPECULAR][0]);
                            g_spc = fmin(1.0, cosTheta * m.params[MGL_MAT_SPECULAR][1] * l.params[MGL_LIGHT_SPECULAR][1]);
                            b_spc = fmin(1.0, cosTheta * m.params[MGL_MAT_SPECULAR][2] * l.params[MGL_LIGHT_SPECULAR][2]);
                            
                            r += fmin(1.0, r_amb + r_dif + r_spc);
                            g += fmin(1.0, g_amb + g_dif + g_spc);
                            b += fmin(1.0, b_amb + b_dif + b_spc);
                            
//                            r = 0.5 * (normal.v[0] + 1);
//                            g = 0.5 * (normal.v[1] + 1);
//                            b = 0.5 * (normal.v[2] + 1);
                        }
                    }
                }
                else
                {
                    MGLfloat linearR = alpha * MGL_GET_RED(tri.v[0].color) + beta * MGL_GET_RED(tri.v[1].color) + gamma * MGL_GET_RED(tri.v[2].color);
                    MGLfloat linearG = alpha * MGL_GET_GREEN(tri.v[0].color) + beta * MGL_GET_GREEN(tri.v[1].color) + gamma * MGL_GET_GREEN(tri.v[2].color);
                    MGLfloat linearB = alpha * MGL_GET_BLUE(tri.v[0].color) + beta * MGL_GET_BLUE(tri.v[1].color) + gamma * MGL_GET_BLUE(tri.v[2].color);
                    
                    r = (linearR / linearInvW) / 255.0f;
                    g = (linearG / linearInvW) / 255.0f;
                    b = (linearB / linearInvW) / 255.0f;
                }
                
                r = fminf(1.0, fmaxf(0.0, r));
                g = fminf(1.0, fmaxf(0.0, g));
                b = fminf(1.0, fmaxf(0.0, b));
                
                MGLbyte red = MGL_GET_RED(diffuseColor) * r;
                MGLbyte green = MGL_GET_GREEN(diffuseColor) * g;
                MGLbyte blue = MGL_GET_BLUE(diffuseColor) * b;
                
                p.color = 0xffffffff;
                
                MGL_SET_RED(p.color, red);
                MGL_SET_GREEN(p.color, green);
                MGL_SET_BLUE(p.color, blue);
                
				MGLsize idx = x + y * fragbuf.width;
				
				// Z-buffer comparison
				if (p.z < fragbuf.data[idx].depth ) {
					fragbuf.data[idx].color = p.color;
					fragbuf.data[idx].depth = p.z;
				}
				
				///////////////////////////////////////////////////
				//
				// END HERE
				//
				///////////////////////////////////////////////////
			}
			
		}
	}
	
	
}

/**
 * Fragment shader.
 *
 * Input - frag
 * Output - data
 */
void shadeFragment(const MGLfragment& frag, MGLpixel& data)
{
    data = frag.color;
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
    lightingEnabled = enabled;
}

void initLight(MGLlight light)
{
    std::map<MGLlight, Light>::iterator i = lights.find(light);
    if (i == lights.end())
    {
        // INITIAL LIGHT SETUP
        Light lightToSetup;
        MGLfloat diffuseAmount = light == MGL_LIGHT0 ? 1.0f : 0.0f;
        MGLfloat specularAmount = light == MGL_LIGHT0 ? 1.0f : 0.0f;
        lightToSetup.params[MGL_LIGHT_AMBIENT] = std::vector<float>(3, 0.0f);
        lightToSetup.params[MGL_LIGHT_DIFFUSE] = std::vector<float>(3, diffuseAmount);
        lightToSetup.params[MGL_LIGHT_SPECULAR] = std::vector<float>(3, specularAmount);
        MGLfloat position[4] = {0.0f, 0.0f, 1.0f, 1.0f};
        MGLfloat transformedPosition[4];
        mulMatrixVector(modelViewMatrix, position, transformedPosition);
        lightToSetup.params[MGL_LIGHT_POSITION] = std::vector<float>(transformedPosition, transformedPosition + 3);
        lights[light] = lightToSetup;
        
    }
}

/**
 * Enable or disable the specified individual light.  All lights
 * should be disabled by default.
 */
void mglLightEnabled(MGLlight light, bool enabled)
{
    initLight(light);
    lights[light].enabled = enabled;
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
    shadingMode = mode;
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
    initLight(light);
    if (pname == MGL_LIGHT_POSITION)
    {
        MGLfloat position[4];
        for (int i = 0; i < 3; i++)
        {
            position[i] = values[i];
        }
        position[3] = 1.0f;
        MGLfloat transformedPosition[4];
        mulMatrixVector(modelViewMatrix, position, transformedPosition);
        values = transformedPosition;
    }
    
    lights[light].params[pname] = vector<float>(values, values + 3);
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
    static bool doFirstTimeMaterialSetup = true;
    if (doFirstTimeMaterialSetup)
    {
        doFirstTimeMaterialSetup = false;  // never again
        currentMaterial.params[MGL_MAT_AMBIENT] = vector<MGLfloat>(0.2, 3);
        currentMaterial.params[MGL_MAT_DIFFUSE] = vector<MGLfloat>(0.8, 3);
        currentMaterial.params[MGL_MAT_SPECULAR] = vector<MGLfloat>(0.0, 3);
        currentMaterial.params[MGL_MAT_SHININESS] = vector<MGLfloat>(0.0, 1);
    }
    
    int numValues = 3;
    if (pname == MGL_MAT_SHININESS)
        numValues = 1;
    currentMaterial.params[pname] = vector<MGLfloat>(values, values + numValues);
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
void mglTexturesEnabled(bool enabled)
{
    texturesEnabled = enabled;
}

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
MGLtex_id mglLoadTexture(MGLsize width,
                         MGLsize height,
                         MGLpixel *imageData)
{
    MGLtex_id id = nextTextureId++;
    const Texture t(width, height, imageData);
    textures.insert(pair<MGLtex_id, Texture>(id, t));
    return id;
}

/**
 * Specify the texture which should be applied to newly specified
 * surfaces, using the ID returned by mglLoadTexture().  When a 
 * texture is applied to the surface, it should be modulated 
 * (multiplied) with the material color.  See also: mglTextureSlot.
 * 
 * By default, the texture ID is set to zero (the NULL texture).
 */
void mglUseTexture(MGLtex_id id)
{
    if (textures.find(id) == textures.end())
    {
        MGL_ERROR("An unregistered texture ID was used in a call to mglUseTexture");
    }
    if (textureSlot == MGL_TEX_DIFFUSE)
    {
        currentDiffuseTextureId = id;
    }
    else if (textureSlot == MGL_TEX_SPECULAR)
    {
        currentSpecularTextureId = id;
    }
}

/**
 * Free any memory associated with the specified texture.  A user
 * should call this once she knows a texture will no longer be used.
 * Note that this invalidates the given ID - that is, an error
 * should be thrown if it is later passed to mglUseTexture().
 */
void mglFreeTexture(MGLtex_id id)
{
    Texture t = textures[id];
    delete [] t.data;
    textures.erase(id);
}

/**
 * Should be called just before a call to mglVertex*(); specifies
 * a texture coordinate on the current texture for the vertex
 * which is about to be specified.
 */
void mglTexCoord(MGLfloat x,
                 MGLfloat y)
{
    currentTexCoord.u = x;
    currentTexCoord.v = y;
}

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
void mglTextureSlot(MGLtex_slot slot)
{
    textureSlot = slot;
}

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
    currentNormal = Vector3(x, y, z).normalize();
}

/**
 * Read pixel data starting with the pixel at coordinates
 * (0, 0), up to (width,  height), into the array
 * pointed to by data.  The boundaries are lower-inclusive,
 * that is, a call with width = height = 1 would just read
 * the pixel at (0, 0).
 *
 * Rasterization and z-buffering should be performed when
 * this function is called, so that the data array is filled
 * with the actual pixel values that should be displayed on
 * the two-dimensional screen.
 */
void mglReadPixels(MGLsize width,
                   MGLsize height,
                   MGLpixel *data)
{
    if (hasBegun) MGL_ERROR("mglReadPixels executed after mglBegin.");
    
    MGLfragbuffer fragbuf(width, height);
    
    // Rasterize each triangle from the vertices array
    for (size_t i = 0; i < transformedVertices.size(); i += 3) {
        MGLtriangle tri;
        tri.v[0] = transformedVertices[i];
        tri.v[1] = transformedVertices[i + 1];
        tri.v[2] = transformedVertices[i + 2];
        rasterize(tri, fragbuf);
    }
    
    // Here it is actually similar to per-fragment shader, where you output
    // the final color for each pixel from the fragment data it contains.
    // Currently it simply copies fragment color to the pixel, and you will
    // need to modify that to account for other new attributes you added.
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            shadeFragment(fragbuf.data[i * width + j],
                          data[i * width + j]);
        }
    }
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
    
    printf("---\n");
    
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
    v.u = currentTexCoord.u;
    v.v = currentTexCoord.v;
    
    // TODO: Multiply the normal by the transpose of the inverse of the modelview matrix.
    Matrix3 normalMatrix = matrix3FromMGLMatrix(modelViewMatrix).invert().transpose();
    Vector3 transformedNormal = (normalMatrix * currentNormal).normalize();
    v.nx = transformedNormal.v[0];
    v.ny = transformedNormal.v[1];
    v.nz = transformedNormal.v[2];
    
    // The following are all linear in cartesian space 
    v.f_prime_u = v.u / v.w;
    v.f_prime_v = v.v / v.w;

    v.f_prime_nx = v.nx / v.w;
    v.f_prime_ny = v.ny / v.w;
    v.f_prime_nz = v.nz / v.w;
    
    v.ex = v.x;
    v.ey = v.y;
    v.ez = v.z;
    v.f_prime_ex = v.ex / v.w;
    v.f_prime_ey = v.ey / v.w;
    v.f_prime_ez = v.ez / v.w;
    
    v.g_prime = 1.0 / v.w;  // linear in cartesian space, used to recover values from f_primes after linearly interpolating everything
     
    v.diffuseTexture = textures[currentDiffuseTextureId];
    v.specularTexture = textures[currentSpecularTextureId];
    
    MGL_SET_RED(v.color, (MGLbyte)(MGL_GET_RED(curColor) / v.w));
    MGL_SET_GREEN(v.color, (MGLbyte)(MGL_GET_GREEN(curColor) / v.w));
    MGL_SET_BLUE(v.color, (MGLbyte)(MGL_GET_BLUE(curColor) / v.w));
    
    v.material = currentMaterial;
    
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
    
    if (curMatrixMode == MGL_MODELVIEW) modelViewStack.push(modelViewMatrix);
    else projectionStack.push(projectionMatrix);
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
    
    MGLmatrix result;
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            MGLfloat sum = 0;
            for (int k = 0; k < 4; ++k) {
                sum += curMatrix->m[k * 4 + j] * matrix[i * 4 + k];
            }
            result.m[i * 4 + j] = sum;
        }
    }
    
    *curMatrix = result;
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
    mglMultMatrix(tMat.m);
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
    mglMultMatrix(rMat.m);
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
    mglMultMatrix(sMat.m);
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
