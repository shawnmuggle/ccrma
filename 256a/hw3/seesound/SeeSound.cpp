#include "RtAudio.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <deque>

#ifdef __MACOSX_CORE__
  // note: for mac only
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

#include "chuck_fft.h"
#include <pthread.h>
#include "gfx.h"

using namespace std;

#define SAMPLE float

pthread_mutex_t buf_mutex;
pthread_mutex_t particles_mutex;

// width and height of the window
GLsizei g_width = 800;
GLsizei g_height = 600;

unsigned int g_sample_rate = 0;

// global buffer
SAMPLE **g_freq_buffers;
unsigned int g_num_freq_buffers = 40;
unsigned int g_freq_buffer_front_index = 0;
long g_freq_buffer_size; // size of buffer in samples

float *g_window;

float g_interesting_freqs_sum = 0;
float g_prev_interesting_freqs_sum = 0;
float g_bass_freqs_sum = 0;

float g_average_bin = 0;

GLfloat z_rot = 0.0;
GLfloat y_rot = 0.0;
GLfloat x_rot = 0.0;

class Particle
{
public:
  Particle(Vector3D *position, Vector3D *velocity, Vector3D *color) : position(position), velocity(velocity), color(color) {}
  ~Particle() { delete position; delete velocity; delete color;}

public:
  void update_position() 
  { 
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
  }

public:
  Vector3D *color;
  Vector3D *position;
  Vector3D *velocity;
};

deque<Particle *> particles;



//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void idleFunc( );
void displayFunc( );
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );

int AudioCallback(void *output_buffer, 
		  void *input_buffer, 
		  unsigned int n_buffer_frames,
		  double stream_time, 
		  RtAudioStreamStatus status, 
		  void *userData )
{
  SAMPLE *buffy = (SAMPLE *)input_buffer;
  pthread_mutex_lock(&buf_mutex);    
  SAMPLE *freq_buf = g_freq_buffers[g_freq_buffer_front_index];
  for( int i = 0; i < n_buffer_frames; i += 2 ) {
    
    //((SAMPLE *)output_buffer)[i] = buffy[i];

    freq_buf[i/2] = buffy[i];
  }
  
  apply_window(freq_buf, g_window, g_freq_buffer_size);
  rfft(freq_buf, g_freq_buffer_size / 2, 1);
  g_freq_buffer_front_index++;
  g_freq_buffer_front_index %= g_num_freq_buffers;
  
  pthread_mutex_unlock(&buf_mutex);
  
  
  float bin_total = 0;
  float new_interesting_freqs_sum = 0;
  float new_bass_freqs_sum = 0;
  for (int i = 0; i < g_freq_buffer_size / 2; i++) {
    float abs_val = cmp_abs(((complex *)freq_buf)[i]);
    new_interesting_freqs_sum += abs_val;
    if (i  < 10) {
      new_bass_freqs_sum += abs_val;
    }
    bin_total += i * abs_val;
  }
  g_interesting_freqs_sum = (g_prev_interesting_freqs_sum + new_interesting_freqs_sum) / 2;
  
  
  pthread_mutex_lock(&particles_mutex);
  if (new_bass_freqs_sum >  g_bass_freqs_sum * 3) {
    Vector3D *pos = new Vector3D(0, 0, 0);
    Vector3D *vel = new Vector3D(0, 0, 1);
    Vector3D *color = new Vector3D(rand() / (double)RAND_MAX,
				   rand() / (double)RAND_MAX,
				   rand() / (double)RAND_MAX);
    Particle *particle = new Particle(pos, vel, color);
    particles.push_back(particle);
    
    if (particles.size() > 20) {
      particles.pop_front();
    }
  }
  pthread_mutex_unlock(&particles_mutex);
  g_bass_freqs_sum = new_bass_freqs_sum;
  
  g_prev_interesting_freqs_sum = new_interesting_freqs_sum;
  
  g_average_bin = bin_total / new_interesting_freqs_sum;
  
  /*
    SAMPLE output_sample = 0;
    
    SAMPLE input_sample = ((SAMPLE *) input_buffer)[i/2];
    
    //g_buffer[i] = input_sample;
    ((SAMPLE *)output_buffer)[i++] = input_sample;
    ((SAMPLE *)output_buffer)[i++] = input_sample;
  */

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
      input_params.nChannels = 2;
    }

    if (chose_output_device && chose_input_device) {
      //break;
    }
  }

  cout << "Using output device \"" << audio->getDeviceInfo(output_params.deviceId).name << "\",  " << 
    output_params.nChannels << " output channels." << endl;
  cout << "Using input device \"" << audio->getDeviceInfo(input_params.deviceId).name << "\", " << 
    input_params.nChannels << " input channels." << endl;

  RtAudio::StreamOptions options;
  options.flags |= RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;

  unsigned int buffer_frames = 1024;

  // allocate global buffers
  g_freq_buffer_size = buffer_frames / 2;
  g_freq_buffers = new SAMPLE *[g_num_freq_buffers];
  for (int i = 0; i < g_num_freq_buffers; i++) {
    g_freq_buffers[i] = new SAMPLE[g_freq_buffer_size];
  }
  g_window = new float[g_freq_buffer_size];
  hanning(g_window, g_freq_buffer_size);

  try {
    audio->openStream( &output_params,
		       &input_params,
		       RTAUDIO_FLOAT32,
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
  pthread_mutex_init(&buf_mutex, NULL);
  pthread_mutex_init(&particles_mutex, NULL);

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

  GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};		// Fog Color
  glClearColor(1.0f,1.0f,1.0f,1.0f);	  // We'll Clear To The Color Of The Fog ( Modified )
  
  glFogi(GL_FOG_MODE, GL_LINEAR);		// Fog Mode
  glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
  glFogf(GL_FOG_DENSITY, 0.7f);				// How Dense Will The Fog Be
  glHint(GL_FOG_HINT, GL_NICEST);			// Fog Hint Value
  glFogf(GL_FOG_START, 2.0f);				// Fog Start Depth
  glFogf(GL_FOG_END, 30.0f);				// Fog End Depth
  glEnable(GL_FOG);					// Enables GL_FOG
  
  glEnable(GL_LINE_SMOOTH);

  glShadeModel (GL_SMOOTH);

  GLfloat light0_position[] = { 1.0, 1.0, 3.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glEnable(GL_LIGHT0);

  GLfloat light1_position[] = { -2.0, -2.0, 3.0, 0.0 };
  GLfloat diffuseLight1[] = {1.0, 1.0, 1.0, 1.0};  
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  glEnable(GL_LIGHT1);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  SetUpAudio();
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
    gluLookAt( 2.0f, 2.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
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
  static GLfloat time_accumulator = 0.0;

  XGfx::getCurrentTime(true);
  //cout << "ACTUAL DELTA: " << XGfx::delta() << endl;
  time_accumulator += XGfx::delta();
  if (time_accumulator < 1.0 / 60.0) {
    //cout << "HERP" << endl;
    return;
  }
  //cout << "RENDERTIME at time: " << time_accumulator << endl;
  time_accumulator = 0.0;
  XGfx::resetCurrentTime();

  // local state
  static GLfloat c = 0.0f;
  static GLfloat radius = 1.0f;
  
  // clear the color and depth buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glPushMatrix(); // Go to back of waterfall
  glTranslatef(0, 0, -(GLfloat)g_num_freq_buffers * 0.5);
  
  pthread_mutex_lock(&buf_mutex);
  GLfloat x;
  for (int buf_idx = 0; buf_idx < g_num_freq_buffers; buf_idx++) {
    // push the matrix
    glPushMatrix();
    // line width
    //glLineWidth( 5 * buf_idx / g_num_freq_buffers );
    
    glTranslatef(0.0, 0.0, (GLfloat)buf_idx * 0.5);
    x = -5;
    GLfloat xinc = ::fabs(2*x / (g_freq_buffer_size / 4));
    
    // go
    glBegin( GL_LINE_STRIP );
    // loop through global buffer
    float prev_y_val = 0;
    
    complex *freq_buf = (complex *)g_freq_buffers[(g_freq_buffer_front_index + buf_idx) 
						  % g_num_freq_buffers];
    
    //SAMPLE *freq_buf = g_freq_buffers[(g_freq_buffer_front_index + buf_idx + 1) 
    //% g_num_freq_buffers];
    

    if (buf_idx == g_num_freq_buffers - 1) {
      glLineWidth(2.0);
      glColor3f(0.0, 0.4, 0.0);
    } else {
      glLineWidth(1.0);
      glColor4f( 0.8 * (0.2 * (sin(c+.8)+1)/2), 0.8 + (0.2 * (sin(c+.01)+1)/2), 0.7 + 0.3 * ((sin(c+.5)+1)/2), 0.5);
    }
    
    for( int i = 0; i < g_freq_buffer_size / 4; i++ ) {
      // set the next vertex

      //float y_val = log10(cmp_abs(freq_buf[i])) + 3;
      float y_val = 20 * pow(cmp_abs(freq_buf[i]), 0.5) - 1;
      
      glVertex2f( x, 1.2 * (y_val + prev_y_val) / 2);
      prev_y_val = y_val;
      //cout << "here's a value: " << y_val << endl;
      
      // increment x
      x += xinc;
    }
    // done
    glEnd();
    
    // pop
    glPopMatrix();
  }
  
  //cout << "Interesting freq mag sum: " << g_interesting_freqs_sum << endl;
  
  // increment color
  c += .001;
  glPopMatrix();
  pthread_mutex_unlock(&buf_mutex);
  
  
  glEnable ( GL_LIGHTING ) ;


  // PARTICLES
  pthread_mutex_lock(&particles_mutex);

  deque<Particle *>::iterator itr=particles.begin();
  while(itr != particles.end()) {
    Particle *particle = (*itr);
    //cout << particle->position->x << endl;;
    particle->update_position();
    
    glPushMatrix(); // beat sphere
    
    glTranslatef(particle->position->x * 0.1, 
		 particle->position->y * 0.1, 
		 particle->position->z * 0.1);
		 
    glColor4f(0.5 + 0.2 * particle->color->x,
	      0.1 + 0.1 * particle->color->y,
	      0.8 + 0.2 * particle->color->z,
	      0.3);
    
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable ( GL_COLOR_MATERIAL ) ;
    
    //glutSolidSphere(pow(particle->position->z, -0.9), 32, 32);
    glutSolidTeapot(pow(particle->position->z, -0.9));    

    glDisable ( GL_COLOR_MATERIAL ) ;
    
    glPopMatrix(); // beat sphere
  
    ++itr;
  }
  

  pthread_mutex_unlock(&particles_mutex);


  GLfloat speed_mult = radius * 2;
  x_rot += 0.47 * speed_mult;
  y_rot += 0.03 * speed_mult;
  z_rot += 0.11 * speed_mult;
  
  glPushMatrix(); // freq sphere
  
  glRotatef(x_rot, 1.0, 0.0, 0.0);
  glRotatef(y_rot, 0.0, 1.0, 0.0);
  glRotatef(z_rot, 0.0, 0.0, 1.0);
  glTranslatef(radius * 1.2, 0.0, 0.0);
  
  glColor4f(20 * g_average_bin / g_freq_buffer_size,
	    0.1,
	    0.1,
	    0.6);

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable ( GL_COLOR_MATERIAL ) ;

  glutSolidSphere(0.1, 16, 16);

  glDisable ( GL_COLOR_MATERIAL ) ;
  
  glPopMatrix(); // freq sxphere
  
  glPushMatrix(); // freq sphere
  
  glRotatef(x_rot, 0.2, 1.0, 0.0);
  glRotatef(y_rot, 0.0, 0.3, 0.8);
  glRotatef(z_rot, 1.0, 0.0, 0.4);
  glTranslatef(radius * 1.2, 0.0, 0.0);
  
  glColor4f(0.1,
	    20 * g_average_bin / g_freq_buffer_size,
	    0.1,
	    0.6);

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable ( GL_COLOR_MATERIAL ) ;

  glutSolidSphere(0.1, 16, 16);
  
  glDisable ( GL_COLOR_MATERIAL ) ;

  glPopMatrix(); // freq sxphere
  
  glPushMatrix(); // freq sphere
  
  glRotatef(x_rot, 0.0, 0.0, 0.8);
  glRotatef(y_rot, 0.8, 0.4, 0.5);
  glRotatef(z_rot, 0.2, 0.1, 0.3);
  glTranslatef(radius * 1.2, 0.0, 0.0);
  
  glColor4f(0.1,
	    0.1,
	    20 * g_average_bin / g_freq_buffer_size,
	    0.6);

  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable ( GL_COLOR_MATERIAL ) ;

  glutSolidSphere(0.1, 16, 16);
  
  glDisable ( GL_COLOR_MATERIAL ) ;
  
  glPopMatrix(); // freq sxphere



  glPushMatrix(); // power sphere
  
  glTranslatef(0.0, 0.0, 0.0);
  glColor4f(0.0 + g_interesting_freqs_sum, 
	    0.5 + g_interesting_freqs_sum / 2,
	    1.0 - g_interesting_freqs_sum,
	    0.3);
    
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable ( GL_COLOR_MATERIAL ) ;

  GLfloat new_radius = pow(g_interesting_freqs_sum + 1, 2);
  radius *= 0.8;
  if (new_radius > radius) {
    radius = new_radius;
  }
  glutSolidSphere(radius, 32, 32);
  
  glDisable ( GL_COLOR_MATERIAL ) ;
  
  glPopMatrix(); // power sphere
  
  glDisable ( GL_LIGHTING ) ; 

  // flush!
  glFlush( );
  // swap the double buffer
  glutSwapBuffers( );
}

