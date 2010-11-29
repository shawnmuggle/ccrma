/*
#ifdef __MAC__
  // note: for mac only
  #include <GLUT/glut.h>
  #include <OpenGL/OpenGL.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif
*/

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "RtAudio.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>

#include "gfx.h"
#include "vector3d.h"

//#include "Quaternion.h"
//#include "MathsStruct.h"

#include "ugens.h"

//#include "vector_math.h"

#include "pthread.h"
#include <stdexcept>
#include <string.h>
#include <vector>
#include <set>

//#include "OgreQuaternion.h"
//#include "OgreVector3.h"

using namespace std;

int g_idle_count = 0, g_display_count = 0, g_actual_display_count = 0;

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

RtAudio audio;

Vector3D position;
float g_pitch = 0;
float g_yaw = 0;


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
  gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 0.1, 1000.0 );
  
  // set the matrix mode to modelview
  glMatrixMode( GL_MODELVIEW );
  
  // load the identity matrix
  glLoadIdentity( );
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int key_x, int key_y )
{
  static float movement_scale = 0.4;
  float rad_pitch = DEGTORAD(g_pitch + 90);
  float rad_yaw = DEGTORAD(g_yaw);

  switch( key )
    {
    case 10:
    case 13:
      break;

    case 's':
      break;
    case 'w':
      position.z += movement_scale * sin(rad_pitch) * cos(rad_yaw);
      position.x -= movement_scale * sin(rad_pitch) * sin(rad_yaw);
      position.y -= movement_scale * cos(rad_pitch);
      break;

    case 'Q':
    case 'q':
      cout << "Idle: " << g_idle_count << ", display: " << g_display_count << ", actual display: " << g_actual_display_count << endl;
      exit(1);
    break;
    }
  
  glutPostRedisplay( );
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

  if (time - g_prev_motion_time < 0.2) {

    g_yaw += (x - g_prev_mouse_motion_x) * 0.3;

    g_pitch += (y - g_prev_mouse_motion_y) * 0.3;
    if (g_pitch > 90) g_pitch = 90;
    if (g_pitch < -90) g_pitch = -90;
    
  }

  if( x <= 10 || (y) <= 10 || x >= g_width - 10 || y >= g_height - 10) {
    glutWarpPointer( g_width / 2, g_height / 2 );
    g_prev_mouse_motion_x = g_width / 2;
    g_prev_mouse_motion_y = g_height / 2;

  } else {
    g_prev_mouse_motion_x = x;
    g_prev_mouse_motion_y = y;
  }
  g_prev_motion_time = time;
}

//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
  g_idle_count++;
  
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
  g_display_count++;

  static GLfloat time_accumulator = 0.0;
  static GLfloat long_time_accumulator = 0.0;
  
  XGfx::getCurrentTime(true);
  GLfloat delta = XGfx::delta();
  time_accumulator += delta;
  long_time_accumulator += delta;
  //cout << long_time_accumulator << endl;
  if (time_accumulator < 1.0 / 60.0) {
    return;
  }
  if (long_time_accumulator > 10) {
    long_time_accumulator -= 10;
    //cout << "PING!" << endl;
  }
  time_accumulator = 0.0;
  XGfx::resetCurrentTime();

  g_actual_display_count++;

  // clear the color and depth buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  // ROTATE to mouselook angles
  glPushMatrix();

  // load the identity matrix
  glLoadIdentity( );
 


  glRotatef(g_pitch, 1, 0, 0);		// multiply into matrix
  glRotatef(g_yaw, 0, 1, 0);
  glTranslatef(position.x, position.y, position.z);



  
  glColor3f(0.5, 0.5, 0.5);		// origin
  glutWireCube(10.0);



  glPushMatrix();
  glTranslatef(0.0, 0.0, -5.0);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, 0.0, 5.0);
  glColor3f(1.0, 0.0, 1.0);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, 5.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, -5.0, 0.0);
  glColor3f(0.0, 1.0, 1.0);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-5.0, 0.0, 0.0);
  glColor3f(0.0, 0.0, 1.0);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(5.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 0.0);
  glutSolidCube(1);
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
  
  /*
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glutSolidSphere(100, 32, 32);
  glPopMatrix();
  */

  glPushMatrix();
  glTranslatef(0.0f, -long_time_accumulator * 100, 0.0f);
  glColor4f(0.2f, 0.6f, 0.9f, 0.5f);
  glutSolidCube(400);
  glPopMatrix();


  glPushMatrix();
  


  glPopMatrix();
  







  glPopMatrix(); // MOUSELOOK rotation
	    

  glPushMatrix();

  glLoadIdentity();
  
  glColor3f(1.0, 1.0, 1.0);
  glutWireSphere(10, 4, 4);

  glPopMatrix();






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

int SDL_main(int argc, char* argv[])
{
  for (int i = 0; i < 1000; i++) {
    AddBleep(500 - 1000.0 * rand() / RAND_MAX, 
	     500 - 1000.0 * rand() / RAND_MAX, 
	     500 - 1000.0 * rand() / RAND_MAX);
  }

  /*
  // initialize GLUT
  glutInit( &argc, argv );
  // double buffer, use rgb color, enable depth buffer
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  // initialize the window size
  glutInitWindowSize( g_width, g_height );
  // set the window postion
  glutInitWindowPosition( 300, 0 );
  // create the window
  glutCreateWindow( "Flying Dream" );
  */

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

  /*
  // set the idle function - called when idle
  glutIdleFunc(idleFunc);
  // set the display function - called when redrawing
  glutDisplayFunc(displayFunc);
  // set the reshape function - called when client area changes
  glutReshapeFunc(reshapeFunc);
  // set the keyboard function - called on keyboard events
  glutKeyboardFunc(keyboardFunc);
  // set the mouse function - called on mouse stuff
  glutMouseFunc(mouseFunc);  
  glutMotionFunc(mouseMotionFunc);
  glutPassiveMotionFunc(mousePassiveMotionFunc);

  GLfloat light0_position[] = { 0.0, 0.0, 0.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glEnable(GL_LIGHT0);
  //glEnable ( GL_LIGHTING ) ;

  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_POLYGON_SMOOTH);
  glShadeModel(GL_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  try {
    //SetUpAudio();
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
  */
  return 0;
}
