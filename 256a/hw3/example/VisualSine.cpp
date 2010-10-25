//-----------------------------------------------------------------------------
// name: VisualSine.cpp
// desc: HelloSine, visualized
//
// Stanford University | Music 256a: Music, Computing, and Design
//     http://ccrma.stanford.edu/courses/256a/
//-----------------------------------------------------------------------------
#include "RtAudio.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __MACOSX_CORE__
  // note: for mac only
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

#include <iostream>
using namespace std;

// some defines
#define SAMPLE double
#define MY_PIE 3.14159265358979
#define MY_SRATE 44100

// width and height of the window
GLsizei g_width = 800;
GLsizei g_height = 600;

// globle buffer
SAMPLE * g_buffer;
long g_bufferSize; // size of buffer in samples


// audio callback
int callme( char * buffer, int buffer_size, void * user_data )
{
    SAMPLE * buffy = (SAMPLE *)buffer;
    
    for( int i = 0; i < buffer_size; i++ )
    {
        // copy to global buffer
        g_buffer[i] = buffy[i];
        // zero out out buffer
        buffy[i] = 0.0f;
    }
    
    return 0;
}


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void idleFunc( );
void displayFunc( );
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );


// entry point
int main( int argc, char ** argv )
{
    // initialize GLUT
    glutInit( &argc, argv );
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window postion
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow( "VisualSine" );
    
    // set the idle function - called when idle
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );
    
    // RtAudio pointer
    RtAudio * audio = NULL;
    // buffer size
    int buffer_size = 512;
    
    // create the RtAudio
    try {
        audio = new RtAudio(
            0, // device number of output
            1, // number of output channels
            1, // device number for input
            1, // number of input channels
            RTAUDIO_FLOAT64, // format
            MY_SRATE, // sample rate
            &buffer_size, // buffer size
            8 // number of buffers
        );
    } catch( RtError & err ) {
        err.printMessage();
        exit(1);
    }
    
    // allocate global buffer
    g_buffer = new SAMPLE[buffer_size];
    g_bufferSize = buffer_size;

    // set the callback
    try {
        audio->setStreamCallback( &callme, NULL );
        audio->startStream();
    } catch( RtError & err ) {
        // do stuff
        err.printMessage();
        goto cleanup;
    }

    // let GLUT handle the current thread from here
    glutMainLoop();

    // if we get here, then stop!
    try {
        audio->stopStream();
    } catch( RtError & err ) {
        // do stuff
        err.printMessage();
    }

cleanup:
    audio->closeStream();
    delete audio;
    
    return 0;
}


//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( GLsizei w, GLsizei h )
{
    // save the new window size
    g_width = w; g_height = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 300.0 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}



//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 'Q':
        case 'q':
            exit(1);
            break;
    }
    
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON )
    {
        // when left mouse button is down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        // when right mouse button down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else
    {
    }
    
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{
    // local state
    static GLfloat zrot = 0.0f, c = 0.0f;

    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // line width
    glLineWidth( 2.0 );
    // step through and plot the waveform
    GLfloat x = -5;
    // increment
    GLfloat xinc = ::fabs(2*x / (g_bufferSize));
    
    // push the matrix
    glPushMatrix();

    // rotation
    glRotatef( zrot, 0, 0, 1 );
    zrot += .1;
    
    // go
    glBegin( GL_LINE_STRIP );
    // loop through global buffer
    for( int i = 0; i < g_bufferSize; i++ )
    {
        // set the color
        glColor3f( (sin(c)+1)/2, (sin(c*2)+1)/2, (sin(c+.5)+1)/2 );
        // set the next vertex
        glVertex2f( x, 5*g_buffer[i] );
        // increment x
        x += xinc;
    }
    // done
    glEnd();
    
    // increment color
    c += .001;
    
    // pop
    glPopMatrix();

    // flush!
    glFlush( );
    // swap the double buffer
    glutSwapBuffers( );
}
