#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "sgl.h"
#include "st.h"

using namespace std;

STImage* buff;

float angle = 0;
bool incrementAngle = false;
void update( int unused )
{
    if (incrementAngle)
        angle += 1;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );

	// --- Make drawing calls here ---+
    
    sglLoadIdentity();
    sglTranslate(150, 150);
    sglRotate(angle);
//    sglScale(0.25, 0.5);
    sglTranslate(-150, -150);
    

    sglBeginTriangles();
    sglColor(0.9f, 0.1f, 0.1f);
    sglVertex(100.0f, 100.0f);

    sglColor(0.1f, 0.8f, 0.1f);
    sglVertex(200.0f, 100.0f);

    sglColor(0.1f, 0.1f, 0.7f);
    sglVertex(100.0f, 200.f);

    sglColor(0.1f, 0.8f, 0.7f);
    sglVertex(200.0f, 200.0f);

//    sglColor(0.9f, 0.8f, 0.1f);
//    sglVertex(160.f, 410.f);
    sglEnd();

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
	int win_width = 512;
	int win_height = 512;
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
