/**
 * minigl.cpp
 * CS248 Assignment 1, Winter 2010
 * -------------------------------
 * Implement miniGL here.
 * Do not use any additional files
 */

#include <cstdio>
#include <cstdlib>
#include "minigl.h"
#include <stack>
#include <vector>
#include "assert.h"

class Transform
{
  public:
    Transform();
    Transform(const MGLfloat *matrix_pointer);
    void set(int column, int row, MGLfloat value);
    MGLfloat get(int column, int row);
    void print();
  private:
    std::vector<MGLfloat> m;  // Column major!
};

Transform::Transform()
{
  m.resize(16, 0);

  // Default constructor creates the identity matrix
  set(0, 0, 1.0f);
  set(1, 1, 1.0f);
  set(2, 2, 1.0f);
  set(3, 3, 1.0f);
}

Transform::Transform(const MGLfloat *matrix_pointer)
{
  m.resize(16, 0);

  for (int i = 0; i < 16; i++)
  {
    m[i] = matrix_pointer[i];
  }
}

void Transform::set(int column, int row, MGLfloat value)
{
  m[column * 4 + row] = value;
}

MGLfloat Transform::get(int column, int row)
{
  int i = column * 4 + row;
  return m[i];
}

void Transform::print()
{
  for (int row = 0; row < 4; row++)
  {
    for (int column = 0; column < 4; column++)
    {
      printf("%f ", get(column, row));
    }
    printf("\n");
  }
}

class Vertex
{
  public:
    Vertex(MGLfloat x, MGLfloat y, MGLfloat z, MGLfloat w);
    MGLfloat x;
    MGLfloat y;
    MGLfloat z;
    MGLfloat w;
};

Vertex::Vertex(MGLfloat x, MGLfloat y, MGLfloat z, MGLfloat w) : x(x), y(y), z(z), w(w)
{ }

class Color
{
  public:
    Color(MGLbyte r, MGLbyte g, MGLbyte b);
  private:
    MGLbyte r, g, b;
};

Color::Color(MGLbyte r, MGLbyte g, MGLbyte b) : r(r), g(g), b(b)
{ }

class Triangle
{
  public:
    void addVertex(Vertex v);
    bool complete();
  private:
    std::vector<Vertex> vertices;
};

void Triangle::addVertex(Vertex v)
{
  assert(vertices.size() < 3);
  vertices.push_back(v);
}

bool Triangle::complete()
{
  return vertices.size() == 3;
}

// Global state variables
MGLmatrix_mode current_matrix_mode = MGL_MODELVIEW;
std::stack<Transform> modelview_matrix_stack;
std::stack<Transform> projection_matrix_stack;
std::vector<Triangle> triangles;
MGLpoly_mode current_poly_mode;
bool poly_mode_set = false;


// private helpers
std::stack<Transform> &CurrentMatrixStack()
{
  std::stack<Transform> &current_matrix_stack = current_matrix_mode == MGL_MODELVIEW ? modelview_matrix_stack : projection_matrix_stack;

  // Sanity check, we treat the top of the stacks as the current matrix so there should always be something there (specifically, the identity matrix)
  if (!current_matrix_stack.size())
  {
    current_matrix_stack.push(Transform());
  }

  return current_matrix_stack;
}

Transform CurrentMatrix()
{
  std::stack<Transform> &current_matrix_stack = CurrentMatrixStack();
  return current_matrix_stack.top();
}

// Create a new triangle in the triangles vector, and return a reference to it.
Triangle &NewTriangle()
{
  triangles.push_back(Triangle());
  return triangles.back();
}

// Returns a reference to the current triangle-in-progress, creating a new triangle if necessary (empty list, or current triangle is complete)
Triangle &CurrentTriangle()
{
  if (!triangles.size())
  {
    return NewTriangle();
  }
  else
  {
    Triangle &current = triangles.back();
    if (current.complete())
    {
      return NewTriangle();
    }
    else
    {
      return current;
    }
  }
}

/**
 * Standard macro to report errors
 */
inline void MGL_ERROR(const char* description) {
    printf("%s\n", description);
    exit(1);
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
}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
  // Make sure we aren't already in a mglBegin/mglEnd block
  assert(!poly_mode_set);

  // set the new polygon mode
  current_poly_mode = mode;
  poly_mode_set = true;
}

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd()
{
  // Make sure that we are currently in an mglBegin/mglEnd block
  assert(poly_mode_set);
  poly_mode_set = false;
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
  // Assert that we are in a glBegin/glEnd block
  assert(poly_mode_set);

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
  // Assert that we are in a glBegin/glEnd block
  assert(poly_mode_set);
  
  MGLfloat w = 1.0f;

  // Multiply the vector that was passed in by the current modelview matrix
  assert(current_matrix_mode == MGL_MODELVIEW);
  Transform t = CurrentMatrix();
  MGLfloat tx = t.get(0, 0) * x + t.get(1, 0) * y + t.get(2, 0) * z + t.get(3, 0) * w;
  MGLfloat ty = t.get(0, 1) * x + t.get(1, 1) * y + t.get(2, 1) * z + t.get(3, 1) * w;
  MGLfloat tz = t.get(0, 2) * x + t.get(1, 2) * y + t.get(2, 2) * z + t.get(3, 2) * w;
  MGLfloat tw = t.get(0, 3) * x + t.get(1, 3) * y + t.get(2, 3) * z + t.get(3, 3) * w;

  CurrentTriangle().addVertex(Vertex(tx, ty, tz, tw));
}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{
  assert(!poly_mode_set);
  current_matrix_mode = mode;
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{
  assert(!poly_mode_set);
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
  assert(!poly_mode_set);

}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
  assert(!poly_mode_set);
  std::stack<Transform> &current_matrix_stack = CurrentMatrixStack();
  current_matrix_stack.pop();
  current_matrix_stack.push(Transform());
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
  assert(!poly_mode_set);
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
  assert(!poly_mode_set);
  Transform b = CurrentMatrix();
  Transform a = matrix;
  
  // multiply matrices here
  Transform mult;
  mult.set(0, 0, a.get(0, 0) * b.get(0, 0) + a.get(1, 0) * b.get(0, 1) + a.get(2, 0) * b.get(0, 2) + a.get(3, 0) * b.get(0, 3));
  mult.set(0, 1, a.get(0, 1) * b.get(0, 0) + a.get(1, 1) * b.get(0, 1) + a.get(2, 1) * b.get(0, 2) + a.get(3, 1) * b.get(0, 3));
  mult.set(0, 2, a.get(0, 2) * b.get(0, 0) + a.get(1, 2) * b.get(0, 1) + a.get(2, 2) * b.get(0, 2) + a.get(3, 2) * b.get(0, 3));
  mult.set(0, 3, a.get(0, 3) * b.get(0, 0) + a.get(1, 3) * b.get(0, 1) + a.get(2, 3) * b.get(0, 2) + a.get(3, 3) * b.get(0, 3));

  mult.set(1, 0, a.get(0, 0) * b.get(1, 0) + a.get(1, 0) * b.get(1, 1) + a.get(2, 0) * b.get(1, 2) + a.get(3, 0) * b.get(1, 3));
  mult.set(1, 1, a.get(0, 1) * b.get(1, 0) + a.get(1, 1) * b.get(1, 1) + a.get(2, 1) * b.get(1, 2) + a.get(3, 1) * b.get(1, 3));
  mult.set(1, 2, a.get(0, 2) * b.get(1, 0) + a.get(1, 2) * b.get(1, 1) + a.get(2, 2) * b.get(1, 2) + a.get(3, 2) * b.get(1, 3));
  mult.set(1, 3, a.get(0, 3) * b.get(1, 0) + a.get(1, 3) * b.get(1, 1) + a.get(2, 3) * b.get(1, 2) + a.get(3, 3) * b.get(1, 3));

  mult.set(2, 0, a.get(0, 0) * b.get(2, 0) + a.get(1, 0) * b.get(2, 1) + a.get(2, 0) * b.get(2, 2) + a.get(3, 0) * b.get(2, 3));
  mult.set(2, 1, a.get(0, 1) * b.get(2, 0) + a.get(1, 1) * b.get(2, 1) + a.get(2, 1) * b.get(2, 2) + a.get(3, 1) * b.get(2, 3));
  mult.set(2, 2, a.get(0, 2) * b.get(2, 0) + a.get(1, 2) * b.get(2, 1) + a.get(2, 2) * b.get(2, 2) + a.get(3, 2) * b.get(2, 3));
  mult.set(2, 3, a.get(0, 3) * b.get(2, 0) + a.get(1, 3) * b.get(2, 1) + a.get(2, 3) * b.get(2, 2) + a.get(3, 3) * b.get(2, 3));

  mult.set(3, 0, a.get(0, 0) * b.get(3, 0) + a.get(1, 0) * b.get(3, 1) + a.get(2, 0) * b.get(3, 2) + a.get(3, 0) * b.get(3, 3));
  mult.set(3, 1, a.get(0, 1) * b.get(3, 0) + a.get(1, 1) * b.get(3, 1) + a.get(2, 1) * b.get(3, 2) + a.get(3, 1) * b.get(3, 3));
  mult.set(3, 2, a.get(0, 2) * b.get(3, 0) + a.get(1, 2) * b.get(3, 1) + a.get(2, 2) * b.get(3, 2) + a.get(3, 2) * b.get(3, 3));
  mult.set(3, 3, a.get(0, 3) * b.get(3, 0) + a.get(1, 3) * b.get(3, 1) + a.get(2, 3) * b.get(3, 2) + a.get(3, 3) * b.get(3, 3));

  std::stack<Transform> &current_matrix_stack = CurrentMatrixStack();
  current_matrix_stack.pop();
  current_matrix_stack.push(mult);
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x,
                  MGLfloat y,
                  MGLfloat z)
{
  assert(!poly_mode_set);
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
  assert(!poly_mode_set);
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z)
{
  assert(!poly_mode_set);
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
  assert(current_matrix_mode == MGL_PROJECTION);
  assert(!poly_mode_set);
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
  assert(current_matrix_mode == MGL_PROJECTION);
  assert(!poly_mode_set);

  // From Angel Fig. 4.25
  Transform ortho;
  
  // scale to the size of the clipping cube (2x2x2)
  ortho.set(0, 0, 2.0 / (right - left));
  ortho.set(1, 1, 2.0 / (top - bottom));
  ortho.set(2, 2, -2.0 / (far - near));

  // translate to be centered around 0
  ortho.set(3, 0, -(left + right) / (right - left));
  ortho.set(3, 1, -(top + bottom) / (top - bottom));
  ortho.set(3, 2, -(far + near) / (far - near));

  // Turn our nice std::vector back into an array to pass it to mglMultMatrix
  MGLfloat a[16];
  for (int column = 0; column < 4; column++)
  {
    for (int row = 0; row < 4; row++)
    {
      a[column * 4 + row] = ortho.get(column, row);
    }
  }
  mglMultMatrix(a);
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
  
}
