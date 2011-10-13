#include "sgl.h"

#include <vector>

using namespace std;
// --- Do not modify this code ---+
#define IMPLEMENT_THIS_FUNCTION printf("Warning: call to unimplimented function!\n")
STImage* img;
int buffer_width;
int buffer_height;
void setBuffer(STImage* ptr) { img = ptr; }
void setBufferSize(int w, int h) { buffer_width = w; buffer_height = h; }
// --- End of do not modify this code ---+

typedef STVector3 Vertex;  // Using Vector instead of Point just so that I can use ComponentMin/Max
struct Line {
    float a, b, c;
};
struct VertexAttribs {
    Vertex vertex;
    STImage::Pixel color;
};
vector<VertexAttribs> vertices;
STImage::Pixel currentColor;

STTransform3 ctm;

static bool flip = false;
void getCCWVertices(const vector<VertexAttribs> &triangle, VertexAttribs *a, VertexAttribs *b, VertexAttribs *c)
{
    // Grap the vertices in alternating order because every second triangle in a triangle strip is clockwise
    if (!flip)
    {
        *a = triangle[0];
        *b = triangle[1];
        *c = triangle[2];
    }
    else
    {
        *a = triangle[2];
        *b = triangle[1];
        *c = triangle[0];
    }
    flip = !flip;
}

void getBoundingBox(const Vertex &a, const Vertex &b, const Vertex &c, Vertex *bboxMin, Vertex *bboxMax)
{
    *bboxMin = Vertex::ComponentMin(a, Vertex::ComponentMin(b, c));
    *bboxMax = Vertex::ComponentMax(a, Vertex::ComponentMax(b, c));    
}

void makeLineEquation(Vertex v0, Vertex v1, Line &l)
{
    l.a = v1.y - v0.y;
    l.b = v0.x - v1.x;
    l.c = -(l.a * v0.x + l.b * v0.y);
}

int shadow(Line l)
{
    // If this line has a normal vector (a, b) that points right, or straight up, it is in shadow
    return l.a > 0 || (l.a == 0 && l.b > 0);
}

int inside(float e, Line l)
{
    // If the point has been found to be on the line, make sure that the line isn't shadowed
    // Otherwise, just check if the point is on the negative side of the line
    return (e == 0) ? !shadow(l) : (e < 0);
}

// Just plug x and y into the line equation for l
float evaluateLineAtPoint(const Line &l, float x, float y)
{
    return l.a * x + l.b * y + l.c;
}

// This method basically determines the how far (x, y) is from each line in the triangle, as a percent of the distance to the opposite point from that line.
// This is equivalent to the area proportion calculation method from lecture
void barymetricCoordinatesFromBook(const Line &l0, const Line &l1, const Line &l2, 
                                   const Vertex &a, const Vertex &b, const Vertex&c,
                                   const float x, const float y,
                                   float *alpha, float *beta, float *gamma)
{
    // Triangle vertex a is opposite Line l0, b is opposite l1, and c is opposite l2
    *alpha = evaluateLineAtPoint(l0, x, y) / evaluateLineAtPoint(l0, a.x, a.y);
    *beta = evaluateLineAtPoint(l1, x, y) / evaluateLineAtPoint(l1, b.x, b.y);
    *gamma = evaluateLineAtPoint(l2, x, y) / evaluateLineAtPoint(l2, c.x, c.y);;
}

void renderTriangle(const vector<VertexAttribs> &triangle)
{
    if (triangle.size() != 3)
    {
        printf("Tried to render a triangle with less than 3 vertices!\n");
        return;
    }
    
    VertexAttribs a, b, c;
    getCCWVertices(triangle, &a, &b, &c);
    
    Vertex bboxMin, bboxMax;
    getBoundingBox(a.vertex, b.vertex, c.vertex, &bboxMin, &bboxMax);
    
    Line l0, l1, l2;    
    makeLineEquation(a.vertex, b.vertex, l2);
    makeLineEquation(b.vertex, c.vertex, l0);
    makeLineEquation(c.vertex, a.vertex, l1);
    
    // We take the ceilings of the bounding box values because we are drawing from the lower left (lesser x & y values)
    // and so only point up and to the right of the specified vertices can be within the triangle.
    for (int y = ceil(bboxMin.y); y < ceil(bboxMax.y); y++)
    {
        for (int x = ceil(bboxMin.x); x < ceil(bboxMax.x); x++)
        {
            float e0 = evaluateLineAtPoint(l0, x, y);  // l0.a * x + l0.b * y + l0.c;
            float e1 = evaluateLineAtPoint(l1, x, y);  // l1.a * x + l1.b * y + l1.c;
            float e2 = evaluateLineAtPoint(l2, x, y);  // l2.a * x + l2.b * y + l2.c;
            if (inside(e0, l0) && inside(e1, l1) && inside(e2, l2))
            {
                float alpha, beta, gamma;
                barymetricCoordinatesFromBook(l0, l1, l2, a.vertex, b.vertex, c.vertex, x, y, &alpha, &beta, &gamma);
                
                // We are using the first pixel's color only until we have interpolation!
                STImage::Pixel color = STImage::Pixel((STImage::Pixel::Component)(a.color.r * alpha + b.color.r * beta + c.color.r * gamma),
                                                      (STImage::Pixel::Component)(a.color.g * alpha + b.color.g * beta + c.color.g * gamma),
                                                      (STImage::Pixel::Component)(a.color.b * alpha + b.color.b * beta + c.color.b * gamma));
                
                img->SetPixel(x, y, color );
            }
        }
    }
}

void renderTriangles()
{
	if (vertices.size() < 3)
    {
        printf("Tried to render triangles when there were less than 3 vertices!\n");
        return;
    }
    
    vector<VertexAttribs> triangle;
    for (vector<VertexAttribs>::iterator i = vertices.begin(); i != vertices.end(); i++)
    {
        VertexAttribs v = *i;
        triangle.push_back(v);
        
        if (triangle.size() > 3)
        {
            triangle.erase(triangle.begin());
        }
        
        if (triangle.size() >= 3)
        {
            renderTriangle(triangle);
        }
    }
}

void sglBeginTriangles()
{
    vertices.clear();
}

void sglEnd()
{
	renderTriangles();
}

void sglLoadIdentity()
{
    ctm.loadIdentity();
}

void sglScale(SGLfloat xscale, SGLfloat yscale)
{
	IMPLEMENT_THIS_FUNCTION;
}

void sglTranslate(SGLfloat xtrans, SGLfloat ytrans)
{
	IMPLEMENT_THIS_FUNCTION;
}

void sglRotate(SGLfloat angle)
{
	IMPLEMENT_THIS_FUNCTION;
}

void sglPushMatrix()
{
	IMPLEMENT_THIS_FUNCTION;
}

void sglPopMatrix()
{
	IMPLEMENT_THIS_FUNCTION;
}

void sglVertex(SGLfloat x, SGLfloat y)
{
    VertexAttribs v;
    
    printf("VERTEX: %f, %f\n", x, y);
    printf("CTM: %f, %f, %f\n", ctm.matrix.elements[0][0], ctm.matrix.elements[0][1], ctm.matrix.elements[0][2]);
    
    v.vertex = ctm * Vertex(x, y, 1.0f);
    
    printf("NEW VERTEX: %f, %f, %f\n", v.vertex.x, v.vertex.y, v.vertex.z);
    printf("--------\n");
    
    v.color = currentColor;
    vertices.push_back(v);
}

void sglColor(SGLfloat r, SGLfloat g, SGLfloat b)
{
    currentColor = STImage::Pixel(r * 255, g * 255, b * 255);
}
