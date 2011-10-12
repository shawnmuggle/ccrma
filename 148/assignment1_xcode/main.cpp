#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <sstream>

#include "Art1.h"
#include "Art2.h"
#include "Art3.h"

// Question 1: In a GLUT program, how is control passed
// back to the programmer?  How is this set up during
// initialization?

int win_width = 1680;
int win_height = 1050;
//int win_height = 800;

Art1 *art1;
Art2 *art2;
Art3 *art3;

bool drawArt1 = false;
bool drawArt2 = false;
bool drawArt3 = true;

void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    
    if (drawArt3)
        art3->draw();
    if (drawArt2)
        art2->draw();
    if (drawArt1)
        art1->draw();
    
    glutSwapBuffers();
}

void update( int unused )
{
    art1->update();
    art2->update();
    art3->update();
    glutPostRedisplay();
    glutTimerFunc(16.0f, update, 0);
}

void reshape( int w, int h )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    // Question 3: What do the calls to glOrtho()
    // and glViewport() accomplish?
    glOrtho( 0., win_width, 0., win_height, -1., 1. );
    glViewport( 0, 0, w, h );
    
    win_width = w;
    win_height = h;
    
    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
    switch(key) {
        case 27: // Escape key
            exit(0);
            break;
        case '1':
            drawArt1 = !drawArt1;
            break;
        case '2':
            drawArt2 = !drawArt2;
            break;
        case '3':
            drawArt3 = !drawArt3;
            break;
        default:
            break;    
    }
}

void mouseButton(int button, int state, int x, int y)
{
    art3->mouseButton(button, state, x, win_height - y);
}

void mouseMotion(int x, int y)
{
//    printf("Mouse motion! %d %d\n", x, y);
    art1->setTargetPoint(x, win_height - y);
    art2->setTargetPoint(x, win_height - y);
    art3->setTargetPoint(x, win_height - y);
}

int main (int argc, char *argv[])
{
    srand(time(NULL));

    glutInit( &argc, argv );
    // Question 2: What does the parameter to glutInitDisplayMode()
    // specify?
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    
    std::stringstream s;
    s << win_width << "x" << win_height << ":32@75";
    glutGameModeString( s.str().c_str() );  // "1680x1050:32@75"
    glutEnterGameMode();
    
//    glutInitWindowSize( win_width, win_height );
//    glutCreateWindow( "Intro Graphics Assignment 1" );
    
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouseButton );
    glutMotionFunc( mouseMotion );
    glutPassiveMotionFunc( mouseMotion );

//    glutIdleFunc( update );
    glutTimerFunc(16.0f, update, 0);

    art1 = new Art1(win_width, win_height);
    art2 = new Art2(win_width, win_height);
    art3 = new Art3(win_width, win_height);
    
    glClearColor(0.95, 0.95, 0.95, 1.0);
    glutMainLoop();
}
