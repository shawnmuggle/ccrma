#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "sgl.h"
#include "st.h"

using namespace std;

class MRSquare {
    float size;
    float angle;
    float angleIncrement;
    float x, y;
    
public:
    
    MRSquare(float aX, float aY)
    {
        size = 50 + 100 * (rand() / (float)RAND_MAX);
        angle = 0;
        angleIncrement = 0.2 * (rand() / (float)RAND_MAX);
        x = aX;
        y = aY;
    }
    
    void incrementAngle()
    {
        angle += angleIncrement;
    }
    
    void draw()
    {
        sglTranslate(x, y);
        sglRotate(angle);
        sglScale(1.0 + 0.1 * sin(angle), 1.0 + 0.4 * cos(angle));
        sglTranslate(-x, -y);
        
        sglBeginTriangles();
        
        sglColor(0.9f, 0.1f, 0.1f);
        sglVertex(x - size / 2, y - size / 2);
        
        sglColor(0.1f, 0.8f, 0.1f);
        sglVertex(x + size / 2, y - size / 2);
        
        sglColor(0.1f, 0.1f, 0.7f);
        sglVertex(x - size / 2, y + size / 2);
        
        sglColor(0.1f, 0.8f, 0.7f);
        sglVertex(x + size / 2, y + size / 2);
        
        sglEnd();
    }
};

vector<MRSquare *> squares;

STImage* buff;

float angle = 0;
bool incrementAngle = false;
void update( int unused )
{
    if (incrementAngle)
    {
        for (vector<MRSquare *>::iterator i = squares.begin(); i != squares.end(); i++)
        {
            MRSquare *square = *i;
            square->incrementAngle();
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
    buff->clear(STImage::Pixel(0, 0, 0));
    
	// --- Make drawing calls here ---+
    sglLoadIdentity();

    for (vector<MRSquare *>::iterator i = squares.begin(); i != squares.end(); i++)
    {
        MRSquare *square = *i;
        sglPushMatrix();
        square->draw();
        sglPopMatrix();
    }
    
	// --- End of drawing calls ------+

	buff->Draw();
	glutSwapBuffers();
}

void reshape( int w, int h )
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  glOrtho( 0., w, 0., h, -1., 1. );
  glViewport( 0, 0, w, h );
  setBufferSize(w, h);

  glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
	switch(key)
	{
        case 27: // Escape key
            exit(0);
            break;
        case 's': // Save
            buff->Save("output.png");
            break;
        case ' ':
            incrementAngle = !incrementAngle;
            break;
	}
}

int main (int argc, char *argv[])
{
    srand(time(NULL));
    
	int win_width = 512;
	int win_height = 512;
    
    for (int i = 0; i < 10; i++)
    {
        squares.push_back(new MRSquare(win_width * (rand() / (float)RAND_MAX), win_height * (rand() / (float)RAND_MAX)));
    }
    
	buff = new STImage(win_width, win_height, STColor4ub(0, 0, 0, 255));
	setBuffer(buff);

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize( win_width, win_height );

	glutCreateWindow( "Intro Graphics Assignment 2" );

	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
    glutTimerFunc(16, update, 0);

	glutMainLoop();

	delete buff;
}
