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

#include "chuck_fft.h"
#include <pthread.h>

// some defines
#define SAMPLE float
#define MY_PIE 3.14159265358979
#define MY_SRATE 44100

pthread_mutex_t buf_mutex;


// width and height of the window
GLsizei g_width = 800;
GLsizei g_height = 600;

// globle buffer
SAMPLE **g_freq_buffers;
unsigned int g_num_freq_buffers = 10;
unsigned int g_freq_buffer_front_index = 0;
long g_freq_buffer_size; // size of buffer in samples

float *g_window;

// audio callback
int callme( char * buffer, int buffer_size, void * user_data )
{
  //cout << "Receiving buffer of size " << buffer_size << endl;
    
  SAMPLE *buffy = (SAMPLE *)buffer;
  SAMPLE *freq_buf = g_freq_buffers[g_freq_buffer_front_index];

  pthread_mutex_lock(&buf_mutex);    
  for( int i = 0; i < buffer_size; i += 2 ) {
    // copy to global buffer
    freq_buf[i/2] = buffy[i];
    // zero out out buffer
    //buffy[i] = 0.0f;
  }
  //cout << "Windowing " << g_freq_buffer_size << " samples in g_buffer" << endl;
  apply_window(freq_buf, g_window, g_freq_buffer_size);
  rfft(freq_buf, g_freq_buffer_size / 2, 1);
  pthread_mutex_unlock(&buf_mutex);
  
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
  pthread_mutex_init(&buf_mutex, NULL);
  
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
  int buffer_size = 2048;
  
  // create the RtAudio
  try {
    audio = new RtAudio(
			0, // device number of output
			2, // number of output channels
			4, // device number for input
			2, // number of input channels
			RTAUDIO_FLOAT32, // format
			MY_SRATE, // sample rate
			&buffer_size, // buffer size
			8 // number of buffers
			);
  } catch( RtError & err ) {
    err.printMessage();
    exit(1);
  }
  
  cout << "Allocating global buffer of size " << buffer_size << endl;
  
  // allocate global buffers
  g_freq_buffer_size = buffer_size / 2;
  g_freq_buffers = new SAMPLE *[g_num_freq_buffers];
  for (int i = 0; i < g_num_freq_buffers; i++) {
    g_freq_buffers[i] = new SAMPLE[g_freq_buffer_size];
  }
  g_window = new float[g_freq_buffer_size];
  hanning(g_window, g_freq_buffer_size);
  
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
    glLineWidth( 1.0 );
    // step through and plot the waveform
    GLfloat x = -5;
    // increment
    GLfloat xinc = ::fabs(2*x / (g_freq_buffer_size / 2));
    
    // push the matrix
    glPushMatrix();

    // rotation
    //glRotatef( zrot, 0, 0, 1 );
    zrot += .1;

    glTranslatef(0.0, 0.0, -10.0);

    // go
    glBegin( GL_LINE_STRIP );
    // loop through global buffer
    pthread_mutex_lock(&buf_mutex);
    float prev_y_val = 0;
    SAMPLE *freq_buf = g_freq_buffers[g_freq_buffer_front_index];
    for( int i = 0; i < g_freq_buffer_size / 2; i++ )
    {
        // set the color
        glColor3f( (sin(c)+1)/2, (sin(c*2)+1)/2, (sin(c+.5)+1)/2 );
        // set the next vertex
	float y_val = -10 * pow(fabs(log10(fabs(freq_buf[i]) + 0.01)), .5) + 13;
        glVertex2f( x, (y_val + prev_y_val) / 2);
	prev_y_val = y_val;
	//cout << "here's a value: " << y_val << endl;
        // increment x
        x += xinc;
    }
    pthread_mutex_unlock(&buf_mutex);
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
