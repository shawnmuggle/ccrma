#include "SeeSound.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

// width and height of the window
GLsizei g_width = 800;
GLsizei g_height = 600;

unsigned int g_sample_rate = 0;

SAMPLE *g_buffer;
unsigned int g_buffer_size;

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
  // clear the color and depth buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  // push the matrix
  glPushMatrix();

  // go
  glBegin( GL_LINE_STRIP );
  // set the color
  glColor3f( 1.0, 0.0, 0.0);
  
  // set the next vertex
  glVertex3f( 1, 1, 1);
  glVertex3f( -1, -1, -1);
  glVertex3f( 1, -1, 1);

  // done
  glEnd();

  // pop
  glPopMatrix();

  // flush!
  glFlush( );
  // swap the double buffer
  glutSwapBuffers( );
}

int AudioCallback(void *output_buffer, 
		  void *input_buffer, 
		  unsigned int n_buffer_frames,
		  double stream_time, 
		  RtAudioStreamStatus status, 
		  void *userData )
{
  for (unsigned int i = 0; i < n_buffer_frames;) {
    SAMPLE output_sample = 0;
    
    SAMPLE input_sample = ((SAMPLE *) input_buffer)[i/2];

    ((SAMPLE *)output_buffer)[i++] = input_sample;
    ((SAMPLE *)output_buffer)[i++] = input_sample;
  }
  
  return 0;
}

void SetUpAudio()
{  
  static RtAudio *audio = new RtAudio();

  static RtAudio::StreamParameters output_params;
  static RtAudio::StreamParameters input_params;

  audio->showWarnings( true );

  // Choose an audio device and a sample rate
  bool chose_output_device = false;
  bool chose_input_device = false;

  unsigned int sample_rate;
  unsigned int devices = audio->getDeviceCount();
  if ( devices < 1 ) {
    cerr << "No audio device found! (You probably need to start JACK)" << endl;
    exit(1);
  }
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++ ) {
    info = audio->getDeviceInfo(i);
    cout << "Looking at device " << info.name << endl;
    cout << "It has " << info.outputChannels << " output channels and " << info.inputChannels << " input channels." << endl;
    if ( info.isDefaultOutput ) {
      cout << "It is apparently the default output device!" << endl;
      chose_output_device = true;
      output_params.deviceId = i;
      output_params.nChannels = 2;

      if (info.sampleRates.size() < 1) {
	cerr << "No supported sample rates found!" << endl;
	exit(1);
      }
      for (int i = 0; i < info.sampleRates.size(); i++) {
	sample_rate = info.sampleRates[i];
	if (sample_rate == 44100 || sample_rate == 48000) {
	  // Found a nice sample rate, stop looking
	  g_sample_rate = sample_rate;
	  break;
	}
      }
      cout << "Using sample rate: " << g_sample_rate << endl;
    }
    if ( info.isDefaultInput ) {
      chose_input_device = true;
      input_params.deviceId = i;
      input_params.nChannels = 1;
    }

    if (chose_output_device && chose_input_device) {
      break;
    }
  }

  cout << "Using output device \"" << audio->getDeviceInfo(output_params.deviceId).name << "\" which has " << 
    output_params.nChannels << " output channels." << endl;
  cout << "Using input device \"" << audio->getDeviceInfo(input_params.deviceId).name << "\" which has " << 
    input_params.nChannels << " input channels." << endl;

  RtAudio::StreamOptions options;
  options.flags |= RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;

  unsigned int buffer_frames = 512;

  g_buffer_size = buffer_frames;
  g_buffer = new SAMPLE[g_buffer_size];
  

  try {
    audio->openStream( &output_params,
		      &input_params,
		      RTAUDIO_FLOAT64,
		      g_sample_rate,
		      &buffer_frames,
		      &AudioCallback,
		      NULL,
		      &options);

    audio->startStream();
  } catch ( RtError &e ) {
    e.printMessage();
    goto cleanup;
  }

  cout << "actually using " << buffer_frames << " buffer frames" << endl;

  glutMainLoop();

  try {
    audio->stopStream();
  }
  catch ( RtError &e ) {
    e.printMessage();
  }

 cleanup:
  if ( audio->isStreamOpen() ) {
    audio->closeStream();
  }
}

int main( int argc, char *argv[])
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
  glutCreateWindow("SeeSound");
    
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

  SetUpAudio();
  return 0;
}
