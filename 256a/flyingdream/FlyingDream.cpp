#ifdef __MAC__
  // note: for mac only
  #include <GLUT/glut.h>
  #include <OpenGL/OpenGL.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

#include "RtAudio.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>

#include "gfx.h"
#include "vector3d.h"
#include "ugens.h"
#include "pthread.h"
#include <stdexcept>
#include <string.h>
#include <vector>
#include <set>

//#include <Foundation/Foundation.h>

using namespace std;

// width and height of the window
GLsizei g_width = 800;
GLsizei g_height = 600;

class Bleep
{
public:
  Bleep(Vector3D *position, Vector3D *color) : 
    position(position)
  {
    id = rand();
    init(color);
  }
  
  ~Bleep() 
  {
    delete position; 
    delete red; delete green; delete blue; 
  }
  void init(Vector3D *color)
  {
    red = new Vector3D(color->x, color->x, 0.1);
    green = new Vector3D(color->y, color->y, 0.1);
    blue = new Vector3D(color->z, color->z, 0.1);
    delete color;
  }
  void update_color() 
  {
    red->interp();
    green->interp();
    blue->interp();
  }
  void flash()
  {
    red->value = 1.0;
    green->value = 1.0;
    blue->value = 1.0;
  }

public:
  Vector3D *red;
  Vector3D *green;
  Vector3D *blue;
  Vector3D *position;
  int id;
};

vector<Bleep *> bleeps;
pthread_mutex_t bleeps_mutex;

RtAudio audio;

Vector3D *g_cam_position;
Vector3D *g_cam_angle;

//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void idleFunc( );
void displayFunc( );
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );

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
  
  // Create Ortho 640x480 View (0,0 At Top Left)
  //glOrtho(0.0f,g_width,g_height,0.0f,-1.0f,300.0f);
  
  // set the matrix mode to modelview
  glMatrixMode( GL_MODELVIEW );
  
  // load the identity matrix
  glLoadIdentity( );
  // position the view point
  gluLookAt(0, 0, 0, 
	    0, 0, 1, 
	    0, 1, 0);
  /*
  gluLookAt(// POSITION
	    g_cam_position->x, 
	    g_cam_position->y, 
	    g_cam_position->z,
	    // CENTER (FOCUS POINT)
	    g_cam_position->x + sin(g_cam_angle->x), 
	    g_cam_position->y - sin(g_cam_angle->y), 
	    g_cam_position->z - cos(g_cam_angle->x), 
	    // UP VECTOR
	    0.0f, 1.0f, 0.0f );
  */
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
  switch( key )
    {
    case 10:
    case 13:
      break;
      /*
	case 'a':
	g_cam_position->x -= 1;
	break;
	case 'd':
	g_cam_position->x += 1;
	break;
      */
    case 's':
      g_cam_position->z += 1;
      break;
    case 'w':
      g_cam_position->x += 1 * sin(g_cam_angle->x);
      g_cam_position->y -= 1 * sin(g_cam_angle->y);
      g_cam_position->z -= 1 * cos(g_cam_angle->z);
      break;
    case 'Q':
    case 'q':
      exit(1);
    break;
    }
  
  glutPostRedisplay( );
}

void specialKeyboardFunc(int key, int x, int y)
{
  if(key == GLUT_KEY_LEFT) {
    g_cam_position->x -= 1;
  }
  else if(key == GLUT_KEY_RIGHT) {
    g_cam_position->x += 1;
  }
  else if(key == GLUT_KEY_DOWN) {
    g_cam_position->z += 1;
  }
  else if(key == GLUT_KEY_UP) {
    g_cam_position->z -= 1;
  }
}

void AddBleep(float x, float y, float z)
{
  Vector3D *color = new Vector3D(rand() / (double)RAND_MAX,
				 rand() / (double)RAND_MAX,
				 rand() / (double)RAND_MAX);
  Bleep *new_bleep = new Bleep(new Vector3D(x, y, z), color);
  bleeps.push_back(new_bleep);
}

//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
  cout << "Click at " << x << ", " << y << endl;
    
  glutPostRedisplay( );
}

void mouseMotionFunc(int x,int y)
{
}

int g_prev_mouse_motion_x = 0;
int g_prev_mouse_motion_y = 0;
double g_prev_motion_time = XGfx::getCurrentTime(true);
void mousePassiveMotionFunc(int x, int y)
{
  int screen_width = glutGet( GLUT_SCREEN_WIDTH );
  int screen_height = glutGet( GLUT_SCREEN_HEIGHT );

  double time = XGfx::getCurrentTime(true);

  //cout << "yup" << endl;
  
  if (time - g_prev_motion_time < 0.2) {
    g_cam_angle->x += (x - g_prev_mouse_motion_x);
    g_cam_angle->y += (y - g_prev_mouse_motion_y);
  }

  if( x <= 10 || (y) <= 10 || x >= g_width - 10 || y >= g_height - 10) {
    //cout << "WOOOO" << endl;
    glutWarpPointer( g_width / 2, g_height / 2 );
    g_prev_mouse_motion_x = g_width / 2;
    g_prev_mouse_motion_y = g_height / 2;

    //      If on Mac OS X, the following will also work (and CGwarpMouseCursorPosition seems faster than glutWarpPointer).
    //CGPoint centerPos = CGPointMake( g_width / 2, g_height / 2 );
    //CGWarpMouseCursorPosition( centerPos );
    // Have to re-hide if the user touched any UI element with the invisible pointer, like the Dock.
    //      CGDisplayHideCursor(kCGDirectMainDisplay);
  } else {
    g_prev_mouse_motion_x = x;
    g_prev_mouse_motion_y = y;
  }
  g_prev_motion_time = time;
  //cout << g_cam_angle->y << endl;
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

void renderSpacedBitmapString(
			float x, 
			float y,
			int spacing, 
			void *font,
			char *string) {
  char *c;
  int x1=x;
  for (c=string; *c != '\0'; c++) {
	glRasterPos2f(x1,y);
    	glutBitmapCharacter(font, *c);
	x1 = x1 + glutBitmapWidth(font,*c) + spacing;
  }
}

//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{
  static GLfloat time_accumulator = 0.0;
  
  XGfx::getCurrentTime(true);
  time_accumulator += XGfx::delta();
  if (time_accumulator < 1.0 / 60.0) {
    return;
  }
  time_accumulator = 0.0;
  XGfx::resetCurrentTime();
  
  // clear the color and depth buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  // ROTATE to mouselook angles
  glPushMatrix();
  // load the identity matrix
  glLoadIdentity( );
  glRotatef(g_cam_angle->x, 0.0, 1.0, 0.0);
  glRotatef(g_cam_angle->y, 1.0, 0.0, 0.0);
  /*
  // position the view point
  gluLookAt(// POSITION
	    g_cam_position->x, 
	    g_cam_position->y, 
	    g_cam_position->z,
	    // CENTER (FOCUS POINT)
	    g_cam_position->x + sin(g_cam_angle->x), 
	    g_cam_position->y - sin(g_cam_angle->y), 
	    g_cam_position->z - cos(g_cam_angle->x), 
	    // UP VECTOR
	    0.0f, 1.0f, 0.0f );
  */
  //glEnable(GL_LIGHTING);

  glPushMatrix();
  glTranslatef(0.0, 0.0, -5.0);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(1, 32, 32);
  glPopMatrix();

  vector<Bleep *>::iterator itr=bleeps.begin();
  while(itr != bleeps.end()) {
    Bleep *bleep = *itr;
    
    glPushMatrix();
    glTranslatef(bleep->position->x, bleep->position->y, bleep->position->z);
    glColor3f(bleep->red->value, bleep->green->value, bleep->blue->value);
    glutSolidSphere(1, 32, 32);
    glPopMatrix();
    
    ++itr;
  }
  

  //glDisable(GL_LIGHTING);
  
  glPopMatrix(); // MOUSELOOK rotation
	    
  // flush!
  glFlush( );
  // swap the double buffer
  glutSwapBuffers( );
}

int AudioCallback(void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
		  double stream_time, RtAudioStreamStatus status, void *userData )
{
  static int tick_count = 0;
  for (unsigned int i = 0; i < n_buffer_frames * 2;) {
    SAMPLE output_sample = 0;

    vector<Bleep *>::iterator itr=bleeps.begin();
    while(itr != bleeps.end()) {
      Bleep *bleep = *itr;

      //output_sample += bleep->ugen->GetSample(tick_count) * 0.1;

      ++itr;
    }
    
    ((SAMPLE *)output_buffer)[i++] = output_sample;
    ((SAMPLE *)output_buffer)[i++] = output_sample;
    ++tick_count;
  }  
  return 0;
}

void SetUpAudio()
{
  RtAudio::StreamParameters output_params;
  RtAudio::StreamParameters input_params;

  audio.showWarnings( true );

  // Choose an audio device and a sample rate
  unsigned int sample_rate;
  unsigned int devices = audio.getDeviceCount();
  if ( devices < 1 ) {
    cerr << "No audio device found! (You probably need to start JACK)" << endl;
    exit(1);
  }
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++ ) {
    info = audio.getDeviceInfo(i);
    if ( info.isDefaultOutput ) {
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
	  break;
	}
      }
      cout << "Using sample rate: " << sample_rate << endl;
    }
    if ( info.isDefaultInput ) {
      input_params.deviceId = i;
      input_params.nChannels = 1;
    }
  }

  cout << "Using output device ID " << output_params.deviceId << " which has " << 
    output_params.nChannels << " output channels." << endl;
  cout << "Using input device ID " << input_params.deviceId << " which has " << 
    input_params.nChannels << " input channels." << endl;

  RtAudio::StreamOptions options;
  options.flags |= RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;

  unsigned int buffer_frames = 256;
  try {
    audio.openStream( &output_params,
		      &input_params,
		      RTAUDIO_FLOAT64,
		      sample_rate,
		      &buffer_frames,
		      AudioCallback,
		      NULL,
		      &options);

    audio.startStream();
  } catch ( RtError &e ) {
    e.printMessage();
  }
}

int main(int argc, char* argv[])
{
  for (int i = 0; i < 200; i++) {
    AddBleep(100.0 * rand() / RAND_MAX, 100.0 * rand() / RAND_MAX, 100.0 * rand() / RAND_MAX);
  }

  g_cam_position = new Vector3D(0, 0, 5);
  g_cam_angle = new Vector3D(M_PI, 0, 0);

  // initialize GLUT
  glutInit( &argc, argv );
  // double buffer, use rgb color, enable depth buffer
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  // initialize the window size
  glutInitWindowSize( g_width, g_height );
  // set the window postion
  glutInitWindowPosition( 100, 100 );
  // create the window
  glutCreateWindow( "Flying Dream" );

#ifdef __MAC__
  const GLint sync = 1;
  CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &sync);
#endif

  /*
  glutGameModeString("800x600:32@60");
  // enter full screen
  if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) 
    glutEnterGameMode();
  else {
    printf("The select mode is not available\n");
    exit(1);	
  }
  */

  // set the idle function - called when idle
  glutIdleFunc(idleFunc);
  // set the display function - called when redrawing
  glutDisplayFunc(displayFunc);
  // set the reshape function - called when client area changes
  glutReshapeFunc(reshapeFunc);
  // set the keyboard function - called on keyboard events
  glutKeyboardFunc(keyboardFunc);
  glutSpecialFunc(specialKeyboardFunc);
  // set the mouse function - called on mouse stuff
  glutMouseFunc(mouseFunc);  
  glutMotionFunc(mouseMotionFunc);
  glutPassiveMotionFunc(mousePassiveMotionFunc);

  GLfloat light0_position[] = { 0.0, 0.0, 0.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glEnable(GL_LIGHT0);
  //glEnable ( GL_LIGHTING ) ;

  glEnable(GL_COLOR_MATERIAL);
  /*
  glEnable(GL_DEPTH_TEST);
  */
  glEnable(GL_BLEND);
  glShadeModel(GL_SMOOTH);

  try {
    SetUpAudio();
  } catch (RtError &e) {
    e.printMessage();
  }

  // let GLUT handle the current thread from here
  glutMainLoop();

  try {
    audio.stopStream();
  }
  catch ( RtError &e ) {
    e.printMessage();
  }
  if ( audio.isStreamOpen() ) {
    audio.closeStream();
  }
  
  return 0;
}
