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
  // local state
  static GLfloat zrot = 0.0f, c = 0.0f;
  
  // clear the color and depth buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  // line width
  glLineWidth( 2.0 );
  // step through and plot the waveform
  GLfloat x = -5;
  // increment
  GLfloat xinc = ::fabs(2*x / g_buffer_size);
  
  // push the matrix
  glPushMatrix();
  
  // rotation
  glRotatef( zrot, 0, 0, 1 );
  zrot += .1;
  
  // go
  glBegin( GL_LINE_STRIP );
  // loop through global buffer
  for( int i = 0; i < g_buffer_size; i++ ) {
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

// AUDIO CALLBACK (static!)
int Synth::AudioCallback(void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
			 double stream_time, RtAudioStreamStatus status, void *userData )
{
  for (unsigned int i = 0; i < n_buffer_frames * 2;) {
    SAMPLE output_sample = 0;
    
    SAMPLE input_sample = ((SAMPLE *) input_buffer)[i/2];

    g_buffer[i] = input_sample;
    ((SAMPLE *)output_buffer)[i++] = input_sample;
    g_buffer[i] = input_sample;
    ((SAMPLE *)output_buffer)[i++] = input_sample;
  }  
  return 0;
}

void Synth::SetUpAudio()
{
  audio.showWarnings( true );


  // THIS ISN'T WORKING --- WHY!??!?!
  // (it seems like RtAudio's detection of which audio device is default output/input is flaky
  // Look in to why or just go back to jack

  /*
  // Choose an audio device and a sample rate
  bool chose_output_device = false;
  bool chose_input_device = false;

  unsigned int sample_rate;
  unsigned int devices = audio.getDeviceCount();
  if ( devices < 1 ) {
    cerr << "No audio device found! (You probably need to start JACK)" << endl;
    exit(1);
  }
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++ ) {
    info = audio.getDeviceInfo(i);
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

  */

  output_params.deviceId = 2;
  output_params.nChannels = 2;
  input_params.deviceId = 0;
  input_params.nChannels = 1;
  g_sample_rate = 44100;

  cout << "Using output device \"" << audio.getDeviceInfo(output_params.deviceId).name << "\" which has " << 
    output_params.nChannels << " output channels." << endl;
  cout << "Using input device \"" << audio.getDeviceInfo(input_params.deviceId).name << "\" which has " << 
    input_params.nChannels << " input channels." << endl;

  RtAudio::StreamOptions options;
  options.flags |= RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;

  unsigned int buffer_frames = 512;

  g_buffer_size = buffer_frames;
  g_buffer = new SAMPLE[g_buffer_size];
  

  try {
    audio.openStream( &output_params,
		      &input_params,
		      RTAUDIO_FLOAT64,
		      g_sample_rate,
		      &buffer_frames,
		      &Synth::AudioCallback,
		      NULL,
		      &options);

    audio.startStream();
  } catch ( RtError &e ) {
    e.printMessage();
    goto cleanup;
  }

  cout << "actually using " << buffer_frames << " buffer frames" << endl;

  glutMainLoop();

  try {
    audio.stopStream();
  }
  catch ( RtError &e ) {
    e.printMessage();
  }

 cleanup:
  if ( audio.isStreamOpen() ) {
    audio.closeStream();
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

  Synth *synth = new Synth();
  synth->SetUpAudio();
  delete synth;
  return 0;
}
