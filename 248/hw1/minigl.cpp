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
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// CLASS DECLARATIONS (I wish we had more than one file)

class Vector4
{
public:
  Vector4();
  Vector4(MGLfloat x, MGLfloat y, MGLfloat z, MGLfloat w);
  void print();

  Vector4 operator *(MGLfloat rhs);
  Vector4 operator /(MGLfloat rhs);

  MGLfloat x;
  MGLfloat y;
  MGLfloat z;
  MGLfloat w;
};
typedef Vector4 Vertex;

class Vector2
{
public:
  Vector2();
  Vector2(MGLfloat x, MGLfloat y);
  Vector2(Vector4 v4);
  MGLfloat x;
  MGLfloat y;
};
typedef Vector2 Point;

class Matrix4
{
public:
  Matrix4();
  Matrix4(const MGLfloat *matrix_pointer); // FOR CONVENIENCE :(
  void loadIdentity();
  void set(int column, int row, MGLfloat value);
  MGLfloat get(int column, int row) const;
  void print();
  void asArray(MGLfloat *a);

  Matrix4 operator*(Matrix4 const & rhs);
  Vector4 operator*(Vector4 const & rhs);

private:
  std::vector<MGLfloat> m;  // Column major!
};
typedef Matrix4 Transform;

class Color
{
  public:
    Color();
    Color(MGLbyte r, MGLbyte g, MGLbyte b);
    MGLpixel asPixel();
  private:
    MGLbyte r, g, b;
};

class Rect2D
{
public:
  Rect2D();
  Rect2D(MGLfloat left, MGLfloat bottom, MGLfloat right, MGLfloat top);
  bool isInside(Point p);
  void print();
  MGLfloat left, bottom, right, top;
};

class Line2D
{
public:
  Line2D();
  Line2D(Point p0, Point p1);
  MGLfloat evaluate(Point p);
  bool isShadowed();
  bool isInside(Point p);
private:
  MGLfloat a, b, c;
};

// This Triangle class will always store its vertices in Normalized Device Coordinates, so we can do checks to see if 2d points on the 
// projection plane are "inside" the triangle
class Triangle
{
public:
  Triangle();
  Triangle(Color c);
  void addVertex(Vertex v);
  bool isComplete();
  bool isInside(Point p, MGLfloat *z);

  Color color;
  std::vector<Vertex> vertices;
  Rect2D boundingBox;
private:
  Rect2D computeBoundingBox();

  MGLfloat ya_yb, xb_xa, xayb, xbya, yc_ya, xa_xc, xcya, xayc;
  MGLfloat gamma_denom, beta_denom;
  Line2D l0, l1, l2;
};

class Quad
{
public:
  Quad(Color c);
  void addVertex(Vertex v);
  bool isComplete();
  Color color;
  Triangle t0, t1; 
};

// CLASS DEFINITIONS

Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }

Vector4::Vector4(MGLfloat x, MGLfloat y, MGLfloat z, MGLfloat w) : x(x), y(y), z(z), w(w) { }

void Vector4::print()
{
  fprintf(stderr, "%f %f %f %f\n", x, y, z, w);
}

Vector4 Vector4::operator *(MGLfloat rhs)
{
  Vector4 result;
  result.x = x * rhs;
  result.y = y * rhs;
  result.z = z * rhs;
  result.w = w * rhs;
  return result;
}

Vector4 Vector4::operator /(MGLfloat rhs)
{
  Vector4 result = *this * (1.0 / rhs);
  return result;
}

Vector2::Vector2() : x(0.0f), y(0.0f) { }

Vector2::Vector2(MGLfloat x, MGLfloat y) : x(x), y(y) { }

Vector2::Vector2(Vector4 v4) : x(v4.x), y(v4.y) { }

Matrix4::Matrix4()
{
  m.resize(16, 0);
  loadIdentity();
}

Matrix4::Matrix4(const MGLfloat *matrix_pointer)
{
  m.resize(16, 0);

  for (int i = 0; i < 16; i++)
  {
    m[i] = matrix_pointer[i];
  }
}

void Matrix4::loadIdentity()
{
  m.resize(16, 0);
  
  // Default constructor creates the identity matrix
  set(0, 0, 1.0f);
  set(1, 1, 1.0f);
  set(2, 2, 1.0f);
  set(3, 3, 1.0f);
}

void Matrix4::set(int column, int row, MGLfloat value)
{
  m[column * 4 + row] = value;
}

MGLfloat Matrix4::get(int column, int row) const
{
  int i = column * 4 + row;
  return m[i];
}

void Matrix4::print()
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

void Matrix4::asArray(MGLfloat *a)
{
  for (int column = 0; column < 4; column++)
  {
    for (int row = 0; row < 4; row++)
    {
      a[column * 4 + row] = get(column, row);
    }
  }
}

Matrix4 Matrix4::operator*(Matrix4 const & rhs)
{
  Matrix4 mult;

  mult.set(0, 0, get(0, 0) * rhs.get(0, 0) + get(1, 0) * rhs.get(0, 1) + get(2, 0) * rhs.get(0, 2) + get(3, 0) * rhs.get(0, 3));
  mult.set(0, 1, get(0, 1) * rhs.get(0, 0) + get(1, 1) * rhs.get(0, 1) + get(2, 1) * rhs.get(0, 2) + get(3, 1) * rhs.get(0, 3));
  mult.set(0, 2, get(0, 2) * rhs.get(0, 0) + get(1, 2) * rhs.get(0, 1) + get(2, 2) * rhs.get(0, 2) + get(3, 2) * rhs.get(0, 3));
  mult.set(0, 3, get(0, 3) * rhs.get(0, 0) + get(1, 3) * rhs.get(0, 1) + get(2, 3) * rhs.get(0, 2) + get(3, 3) * rhs.get(0, 3));

  mult.set(1, 0, get(0, 0) * rhs.get(1, 0) + get(1, 0) * rhs.get(1, 1) + get(2, 0) * rhs.get(1, 2) + get(3, 0) * rhs.get(1, 3));
  mult.set(1, 1, get(0, 1) * rhs.get(1, 0) + get(1, 1) * rhs.get(1, 1) + get(2, 1) * rhs.get(1, 2) + get(3, 1) * rhs.get(1, 3));
  mult.set(1, 2, get(0, 2) * rhs.get(1, 0) + get(1, 2) * rhs.get(1, 1) + get(2, 2) * rhs.get(1, 2) + get(3, 2) * rhs.get(1, 3));
  mult.set(1, 3, get(0, 3) * rhs.get(1, 0) + get(1, 3) * rhs.get(1, 1) + get(2, 3) * rhs.get(1, 2) + get(3, 3) * rhs.get(1, 3));

  mult.set(2, 0, get(0, 0) * rhs.get(2, 0) + get(1, 0) * rhs.get(2, 1) + get(2, 0) * rhs.get(2, 2) + get(3, 0) * rhs.get(2, 3));
  mult.set(2, 1, get(0, 1) * rhs.get(2, 0) + get(1, 1) * rhs.get(2, 1) + get(2, 1) * rhs.get(2, 2) + get(3, 1) * rhs.get(2, 3));
  mult.set(2, 2, get(0, 2) * rhs.get(2, 0) + get(1, 2) * rhs.get(2, 1) + get(2, 2) * rhs.get(2, 2) + get(3, 2) * rhs.get(2, 3));
  mult.set(2, 3, get(0, 3) * rhs.get(2, 0) + get(1, 3) * rhs.get(2, 1) + get(2, 3) * rhs.get(2, 2) + get(3, 3) * rhs.get(2, 3));

  mult.set(3, 0, get(0, 0) * rhs.get(3, 0) + get(1, 0) * rhs.get(3, 1) + get(2, 0) * rhs.get(3, 2) + get(3, 0) * rhs.get(3, 3));
  mult.set(3, 1, get(0, 1) * rhs.get(3, 0) + get(1, 1) * rhs.get(3, 1) + get(2, 1) * rhs.get(3, 2) + get(3, 1) * rhs.get(3, 3));
  mult.set(3, 2, get(0, 2) * rhs.get(3, 0) + get(1, 2) * rhs.get(3, 1) + get(2, 2) * rhs.get(3, 2) + get(3, 2) * rhs.get(3, 3));
  mult.set(3, 3, get(0, 3) * rhs.get(3, 0) + get(1, 3) * rhs.get(3, 1) + get(2, 3) * rhs.get(3, 2) + get(3, 3) * rhs.get(3, 3));

  return mult;
}

Vector4 Matrix4::operator*(Vector4 const & rhs)
{
  Vector4 v;
  v.x = get(0, 0) * rhs.x + get(1, 0) * rhs.y + get(2, 0) * rhs.z + get(3, 0) * rhs.w;
  v.y = get(0, 1) * rhs.x + get(1, 1) * rhs.y + get(2, 1) * rhs.z + get(3, 1) * rhs.w;
  v.z = get(0, 2) * rhs.x + get(1, 2) * rhs.y + get(2, 2) * rhs.z + get(3, 2) * rhs.w;
  v.w = get(0, 3) * rhs.x + get(1, 3) * rhs.y + get(2, 3) * rhs.z + get(3, 3) * rhs.w;
  return v;
}

Color::Color() : r(1), g(1), b(1) { }

Color::Color(MGLbyte r, MGLbyte g, MGLbyte b) : r(r), g(g), b(b) { }

MGLpixel Color::asPixel()
{
  MGLpixel pixel;
  MGL_SET_RED(pixel, r);
  MGL_SET_GREEN(pixel, g);
  MGL_SET_BLUE(pixel, b);
  return pixel;
}

Rect2D::Rect2D() : left(0.0f), bottom(0.0f), right(0.0f), top(0.0f) { }

Rect2D::Rect2D(MGLfloat left, MGLfloat bottom, MGLfloat right, MGLfloat top) : left(left), bottom(bottom), right(right), top(top) { }

bool Rect2D::isInside(Point p)
{
  return p.x > left && p.x <= right && p.y > bottom && p.y <= top;
}

void Rect2D::print()
{
  fprintf(stderr, "Rect: (%f, %f) (%f, %f)\n", left, bottom, right, top);
}

Line2D::Line2D() { }

Line2D::Line2D(Point p0, Point p1)
{
  a = p1.y - p0.y; 
  b = p0.x - p1.x;
  c = -(a * p0.x + b * p0.y);
}

MGLfloat Line2D::evaluate(Point p)
{
  return a * p.x + b * p.y + c;
}

bool Line2D::isShadowed()
{
  return a > 0.0f || (a == 0.0f && b > 0.0f);
}

bool Line2D::isInside(Point p)
{
  MGLfloat e = evaluate(p);
  return e == 0.0f ? !isShadowed() : (e < 0.0f);
}

Triangle::Triangle() { }

Triangle::Triangle(Color c) : color(c) { }

void Triangle::addVertex(Vertex v)
{
  assert(vertices.size() < 3);
  vertices.push_back(v);
  if (vertices.size() == 3)
  {
    // Done with vertices! Cache some data for computing inside points
    boundingBox = computeBoundingBox();

    ya_yb = vertices[0].y - vertices[1].y;
    xb_xa = vertices[1].x - vertices[0].x;
    xayb = vertices[0].x * vertices[1].y;
    xbya = vertices[1].x * vertices[0].y;
    yc_ya = vertices[2].y - vertices[0].y;
    xa_xc = vertices[0].x - vertices[2].x;
    xcya = vertices[2].x * vertices[0].y;
    xayc = vertices[0].x * vertices[2].y;
    gamma_denom = ya_yb * vertices[2].x + xb_xa * vertices[2].y + xayb - xbya;
    beta_denom = yc_ya * vertices[1].x + xa_xc * vertices[1].y + xcya - xayc;

    l0 = Line2D(vertices[1], vertices[2]);
    l1 = Line2D(vertices[2], vertices[0]);
    l2 = Line2D(vertices[0], vertices[1]);
  }
}

bool Triangle::isComplete()
{
  return vertices.size() == 3;
}

Rect2D Triangle::computeBoundingBox()
{
  return Rect2D(MIN(vertices[0].x, MIN(vertices[1].x, vertices[2].x)),
                MIN(vertices[0].y, MIN(vertices[1].y, vertices[2].y)),
                MAX(vertices[0].x, MAX(vertices[1].x, vertices[2].x)),
                MAX(vertices[0].y, MAX(vertices[1].y, vertices[2].y)));
}

bool Triangle::isInside(Point p, MGLfloat *z)
{
  if (!isComplete())
    return false;

  bool insideLines = l0.isInside(p) && l1.isInside(p) && l2.isInside(p);
  if (!insideLines)
    return false;

  MGLfloat gamma = (ya_yb * p.x + xb_xa * p.y + xayb - xbya) / gamma_denom;
  MGLfloat beta = (yc_ya * p.x + xa_xc * p.y + xcya - xayc) / beta_denom;
  MGLfloat alpha = 1.0f - beta - gamma;

  *z = gamma * vertices[2].z + beta * vertices[1].z + alpha * vertices[0].z;

  return gamma >= 0.0f && gamma <= 1.0f && beta >= 0.0f && beta <= 1.0f && alpha >= 0.0f && alpha <= 1.0f;
}

Quad::Quad(Color c) : color(c), t0(c), t1(c) { }

void Quad::addVertex(Vertex v)
{
  if (!t0.isComplete())
  {
    t0.addVertex(v);
    if(t0.isComplete())
    {
      t1.addVertex(t0.vertices[0]);
      t1.addVertex(t0.vertices[2]);
    }
  }
  else if (!t1.isComplete())
  {
    t1.addVertex(v);
  }
  else
  {
    assert(false);
  }
}

bool Quad::isComplete()
{
  return t0.isComplete() && t1.isComplete();
}

// TRANSFORM HELPERS

static Transform orthoTransform(MGLfloat left, MGLfloat right, MGLfloat bottom, MGLfloat top, MGLfloat near, MGLfloat far)
{
  Transform ortho;

  // From Angel Fig. 4.25
  
  // scale to the size of the clipping cube (2x2x2)
  ortho.set(0, 0, 2.0f / (right - left));
  ortho.set(1, 1, 2.0f / (top - bottom));
  ortho.set(2, 2, -2.0f / (far - near));

  // translate to be centered around 0
  ortho.set(3, 0, -(left + right) / (right - left));
  ortho.set(3, 1, -(top + bottom) / (top - bottom));
  ortho.set(3, 2, -(far + near) / (far - near));

  return ortho;
}

static Transform perspectiveTransform(MGLfloat left, MGLfloat right, MGLfloat bottom, MGLfloat top, MGLfloat near, MGLfloat far)
{
  Transform perspective;

  // From http://www.songho.ca/opengl/gl_projectionmatrix.html

  perspective.set(0, 0, (2.0f * near) / (right - left));

  perspective.set(1, 1, (2.0f * near) / (top - bottom));

  perspective.set(2, 0, (right + left) / (right - left));
  perspective.set(2, 1, (top + bottom) / (top - bottom));
  perspective.set(2, 2, -(far + near) / (far - near));
  perspective.set(2, 3, -1.0f);

  perspective.set(3, 2, (-2.0f * far * near) / (far - near));
  perspective.set(3, 3, 0.0f);

  return perspective;
}

static Transform scaleTransform(MGLfloat sx, MGLfloat sy, MGLfloat sz)
{
  Transform scale;
  scale.set(0, 0, sx);
  scale.set(1, 1, sy);
  scale.set(2, 2, sz);
  return scale;
}

static Transform translateTransform(MGLfloat tx, MGLfloat ty, MGLfloat tz)
{
  Transform translate;
  translate.set(3, 0, tx);
  translate.set(3, 1, ty);
  translate.set(3, 2, tz);
  return translate;
}

static Transform rotateTransform(MGLfloat angle, MGLfloat rx, MGLfloat ry, MGLfloat rz)
{
  MGLfloat rad = M_PI * angle / 180.0f;

  MGLfloat magnitude = sqrt(pow(rx, 2.0f) + pow(ry, 2.0f) + pow(rz, 2.0f));
  rx /= magnitude;
  ry /= magnitude;
  rz /= magnitude;

  Transform rotate;
  MGLfloat c = cos(rad);
  MGLfloat s = sin(rad);
  rotate.set(0, 0, c + pow(rx, 2.0f) * (1.0f - c));
  rotate.set(0, 1, ry * rx * (1.0f - c) + rz * s);
  rotate.set(0, 2, rz * rx * (1.0f - c) - ry * s);

  rotate.set(1, 0, rx * ry * (1.0f - c) - rz * s);
  rotate.set(1, 1, c + pow(ry, 2.0f) * (1.0f - c));
  rotate.set(1, 2, rz * ry * (1.0f - c) + rx * s);

  rotate.set(2, 0, rx * rz * (1.0f - c) + ry * s);
  rotate.set(2, 1, ry * rz * (1.0f - c) - rx * s);
  rotate.set(2, 2, c + pow(rz, 2.0f) * (1.0f - c));
  return rotate;
}

// GLOBAL STATE

MGLmatrix_mode current_matrix_mode = MGL_MODELVIEW;
std::stack<Transform> modelview_matrix_stack;
std::stack<Transform> projection_matrix_stack;
std::vector<Triangle> triangles;
std::vector<Quad> quads;
MGLpoly_mode current_poly_mode;
bool poly_mode_set = false;
Color current_color;

// TRANSFORM STACK HELPERS

std::stack<Transform> &modelviewMatrixStack()
{
  // Sanity check, we treat the top of the stacks as the current matrix so there should always be something there (specifically, the identity matrix)
  if (!modelview_matrix_stack.size())
  {
    modelview_matrix_stack.push(Transform());
  }
  return modelview_matrix_stack;  
}

Transform modelviewMatrix()
{
  return modelviewMatrixStack().top();
}

std::stack<Transform> &projectionMatrixStack()
{
  // Sanity check, we treat the top of the stacks as the current matrix so there should always be something there (specifically, the identity matrix)
  if (!projection_matrix_stack.size())
  {
    projection_matrix_stack.push(Transform());
  }
  return projection_matrix_stack;  
}

Transform projectionMatrix()
{

  return projectionMatrixStack().top();
}

std::stack<Transform> &currentMatrixStack()
{
  return current_matrix_mode == MGL_MODELVIEW ? modelviewMatrixStack() : projectionMatrixStack();
}

Transform currentMatrix()
{
  return current_matrix_mode == MGL_MODELVIEW ? modelviewMatrix() : projectionMatrix();
}

// Create a new triangle in the triangles vector, and return a reference to it.
Triangle &newTriangle()
{
  triangles.push_back(Triangle(current_color));
  return triangles.back();
}

Quad &newQuad()
{
  quads.push_back(Quad(current_color));
  return quads.back();
}

// Returns a reference to the current triangle-in-progress, creating a new triangle if necessary (empty list, or current triangle is complete)
Triangle &currentTriangle()
{
  if (!triangles.size())
  {
    return newTriangle();
  }
  else
  {
    Triangle &current = triangles.back();
    if (current.isComplete())
    {
      return newTriangle();
    }
    else
    {
      return current;
    }
  }
}

Quad &currentQuad()
{
  if (!quads.size())
  {
    return newQuad();
  }
  else
  {
    Quad &current = quads.back();
    if (current.isComplete())
    {
      return newQuad();
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
  // Add quad triangles to triangles list for rendering
  for (std::vector<Quad>::iterator i = quads.begin(); i != quads.end(); i++)
  {
    Quad q = *i;
    triangles.push_back(q.t0);
    triangles.push_back(q.t1);
  }

  MGLfloat z_buffer[width * height];
  for (unsigned int i = 0; i < width * height; i++)
  {
    z_buffer[i] = MAXFLOAT;
  }

  for (std::vector<Triangle>::iterator i = triangles.begin(); i != triangles.end(); i++)
  {
    Triangle t = *i;
    Rect2D boundingBox = t.boundingBox;
    MGLsize bottom = ceil(height * (MAX(-1.0f, boundingBox.bottom) + 1) / 2.0);
    MGLsize top = ceil(height * (MIN(1.0f, boundingBox.top) + 1) / 2.0);
    MGLsize left = ceil(width * (MAX(-1.0f, boundingBox.left) + 1) / 2.0);
    MGLsize right = ceil(width * (MIN(1.0f, boundingBox.right) + 1) / 2.0);
    for (MGLsize y = bottom; y < top; y++)
    {
      for (MGLsize x = left; x < right; x++)
      {
        Point p;
        p.x = (2 * (x + 0.5f)) / (MGLfloat) width - 1;
        p.y = (2 * (y + 0.5f)) / (MGLfloat) height - 1;
        MGLfloat z;
        if (t.isInside(p, &z))
        {
          if (z < z_buffer[y * width + x])
          {
            MGLpixel pixel = t.color.asPixel();
            data[y * width + x] = pixel;
            z_buffer[y * width + x] = z;
          }
        }
      }
    }
  }
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

  if (current_poly_mode == MGL_TRIANGLES)
  {
    if (!currentTriangle().isComplete())
    {
      triangles.pop_back();
    }
  }
  else if (current_poly_mode == MGL_QUADS)
  {
    if (!currentQuad().isComplete())
    {
      quads.pop_back();
    }    
  }

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
  Vertex vertex(x, y, z, w);
  Transform modelview = modelviewMatrix();
  Vertex modelviewVertex = modelview * vertex;
  Transform projection = projectionMatrix();
  Vertex projectionVertex = projection * modelviewVertex;
  Vertex ndcVertex = projectionVertex / projectionVertex.w;

  if (current_poly_mode == MGL_TRIANGLES)
  {
    currentTriangle().addVertex(ndcVertex);
  }
  else
  {
    currentQuad().addVertex(ndcVertex);
  }
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
  std::stack<Transform> &current_matrix_stack = currentMatrixStack();
  current_matrix_stack.push(currentMatrix());
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
  assert(!poly_mode_set);
  std::stack<Transform> &current_matrix_stack = currentMatrixStack();
  // We can never do the illegal "empty stack pop" because our accessor guarantees that the stack will 
  //   always have at least the Identity matrix in it
  current_matrix_stack.pop();  
}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
  assert(!poly_mode_set);
  std::stack<Transform> &current_matrix_stack = currentMatrixStack();
  current_matrix_stack.pop();
  current_matrix_stack.push(Transform());  // Default transform constructor creates the identity matrix
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
  Transform newTransform = matrix;

  std::stack<Transform> &current_matrix_stack = currentMatrixStack();
  current_matrix_stack.pop();
  current_matrix_stack.push(newTransform);
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
  Transform newTransform = matrix;
  Transform mult = Transform(currentMatrix() * newTransform);

  std::stack<Transform> &current_matrix_stack = currentMatrixStack();
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
  
  Transform translate = translateTransform(x, y, z);
  // Turn our nice std::vector back into an array to pass it to mglMultMatrix
  MGLfloat a[16];
  translate.asArray(a);
  mglMultMatrix(a);
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

  Transform rotate = rotateTransform(angle, x, y, z);
  // Turn our nice std::vector back into an array to pass it to mglMultMatrix
  MGLfloat a[16];
  rotate.asArray(a);
  mglMultMatrix(a);
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
  Transform scale = scaleTransform(x, y, z);
  // Turn our nice std::vector back into an array to pass it to mglMultMatrix
  MGLfloat a[16];
  scale.asArray(a);
  mglMultMatrix(a);
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

  Transform perspective = perspectiveTransform(left, right, bottom, top, near, far);
  // Turn our nice std::vector back into an array to pass it to mglMultMatrix
  MGLfloat a[16];
  perspective.asArray(a);
  mglMultMatrix(a);
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

  Transform ortho = orthoTransform(left, right, bottom, top, near, far);
  // Turn our nice std::vector back into an array to pass it to mglMultMatrix
  MGLfloat a[16];
  ortho.asArray(a);
  mglMultMatrix(a);
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
  current_color = Color(red, green, blue);
}
