#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Art1.h"

// Question 1: In a GLUT program, how is control passed
// back to the programmer?  How is this set up during
// initialization?

int win_width = 1680;
int win_height = 800;

Art1 *art;

void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    
    art->draw();
    
    glutSwapBuffers();
}

void update( void )
{
    art->update();
    glutPostRedisplay();
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
    }
}

void mouseButton(int button, int state, int x, int y)
{
}

void mouseMotion(int x, int y)
{
//    printf("Mouse motion! %d %d\n", x, y);
    art->setTargetPoint(x, win_height - y);
}

int main (int argc, char *argv[])
{
    srand(time(NULL));
    
    art = new Art1(win_width, win_height);
    
    glutInit( &argc, argv );
    // Question 2: What does the parameter to glutInitDisplayMode()
    // specify?
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( win_width, win_height );
    
    glutCreateWindow( "Intro Graphics Assignment 1" );
    
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouseButton );
    glutMotionFunc( mouseMotion );
    glutPassiveMotionFunc( mouseMotion );
    glutIdleFunc( update );
    
    glClearColor(0.95, 0.95, 0.95, 1.0);
    
    glutMainLoop();
}
