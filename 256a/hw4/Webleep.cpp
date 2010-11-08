#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/ip/IpEndpointName.h"

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

using namespace std;


#define OUTPUT_BUFFER_SIZE 1024

#define IP_MTU_SIZE 1536

#define NUM_COLLISION_SECTIONS 16


char g_hostname[255];
int g_port = 7000;

bool g_connect_mode = false;
char g_other_address[255];
char g_other_address_char_index = 0;
char *g_other_host_and_port[2];
// TODO: INCLUDE LIST OF PEERS FOR MORE THAN 2-PLAYER
// For now, only try to counter-connect if we aren't already connected to someone
bool g_connected = false;

// width and height of the window
GLsizei g_width = 800;
GLsizei g_height = 600;

class Bleep
{
public:
  Bleep(float x, float y, Vector3D *color) : 
    last_loop_seen(-1)
  {
    id = rand();
    init(x, y, color);
  }
  Bleep(float x, float y, Vector3D *color, int id) : 
    last_loop_seen(-1),
    id(id)
  {
    init(x, y, color);
  }
  ~Bleep() 
  {
    delete position; 
    delete red; delete green; delete blue; 
    delete pulse; delete sin; delete env; delete lpf; delete gain;
  }
  void init(float x, float y, Vector3D *color)
  {
    pulse = new Pulse(20 + g_height - y, 0.25);
    sin = new Sine(20 + g_height - y);
    gain = new Gain(0.5);
    lpf = new MovingAverage(10, 10);
    env = new AREnvelope(5, 10000, 1.0);
    lpf->GetAudioFrom(pulse);
    env->GetAudioFrom(lpf);
    gain->GetAudioFrom(sin);
    env->GetAudioFrom(gain);
    ugen = env;

    position = new Vector3D();
    SetPosition(x, y);

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
  void play(int loop_count)
  {
    if (last_loop_seen < loop_count) {
      flash();
      last_loop_seen = loop_count;
      env->Reset();
    }
  }
  void SetPosition(double x, double y)
  {
    position->x = x;
    position->y = y;
    pulse->SetFrequency(20 + g_height - y);
    sin->SetFrequency((20 + g_height - y) / 2);
  }

public:
  Vector3D *red;
  Vector3D *green;
  Vector3D *blue;
  Vector3D *position;
  UGen *ugen;
  int id;

private:
  int last_loop_seen;
  Pulse *pulse;
  Sine *sin;
  Gain *gain;
  AREnvelope *env;
  MovingAverage *lpf;
};

vector<Bleep *> bleeps;
pthread_mutex_t bleeps_mutex;
Bleep *g_dragging_bleep = NULL;

// USE LATER FOR PERFORMAnce (divide screen into sections to do collision detection
//vector<set<Bleep *>> g_collision_sections;

int g_bleep_radius = 10;

int g_loop_count = 0;

RtAudio audio;

pthread_t oscpack_thread;

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
    //gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 300.0 );

    // Create Ortho 640x480 View (0,0 At Top Left)
    glOrtho(0.0f,g_width,g_height,0.0f,-1.0f,300.0f);

    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    //gluLookAt( 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}

void TryToConnectToHost()
{
  g_connect_mode = false;
  g_other_host_and_port[0] = strtok(g_other_address, ":");
  g_other_host_and_port[1] = strtok(NULL, " .,-");
  cout << "Connecting to: " << g_other_host_and_port[0] << " : " << g_other_host_and_port[1] << endl;
  
  int port = atoi(g_other_host_and_port[1]);
  IpEndpointName host( g_other_host_and_port[0], port );
  
  char hostIpAddress[ IpEndpointName::ADDRESS_STRING_LENGTH ];
  host.AddressAsString( hostIpAddress );
  
  char buffer[IP_MTU_SIZE];
  osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
  UdpTransmitSocket socket( host );
  
  p.Clear();
  p << osc::BeginMessage( "/connect" ) << g_hostname << osc::EndMessage;
  socket.Send( p.Data(), p.Size() );
  g_connected = true;
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
  if (g_connect_mode) {
    g_other_address[g_other_address_char_index++] = key;
    g_other_address[g_other_address_char_index] = '\0';
  }

  switch( key )
    {
    case 10:
    case 13:
      TryToConnectToHost();
      break;
    case 'c':
      if (!g_connect_mode) {
	g_connect_mode = true;
	memset(g_other_address, 0, 255);
	g_other_address_char_index = 0;
      }
      break;
    case 'Q':
    case 'q':
      exit(1);
    break;
    }
  
  glutPostRedisplay( );
}

void SendMoveBleep(int id, float x, float y)
{
  int port = atoi(g_other_host_and_port[1]);
  IpEndpointName host( g_other_host_and_port[0], port );
  
  char buffer[IP_MTU_SIZE];
  osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
  UdpTransmitSocket socket( host );
  
  p.Clear();
  p << osc::BeginMessage( "/move" ) << id << x << y << osc::EndMessage;
  socket.Send( p.Data(), p.Size() );
}

void SendEraseBleep(int id)
{
  int port = atoi(g_other_host_and_port[1]);
  IpEndpointName host( g_other_host_and_port[0], port );
  
  char buffer[IP_MTU_SIZE];
  osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
  UdpTransmitSocket socket( host );
  
  p.Clear();
  p << osc::BeginMessage( "/erase" ) << id << osc::EndMessage;
  socket.Send( p.Data(), p.Size() );
}

void SendAddBleep(Bleep *new_bleep)
{
  int port = atoi(g_other_host_and_port[1]);
  IpEndpointName host( g_other_host_and_port[0], port );
  
  char buffer[IP_MTU_SIZE];
  osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
  UdpTransmitSocket socket( host );
  
  p.Clear();
  p << osc::BeginMessage( "/add" ) << new_bleep->id << new_bleep->position->x << new_bleep->position->y << osc::EndMessage;
  socket.Send( p.Data(), p.Size() );
}

void AddBleep(float x, float y, bool send, int id)
{
  Vector3D *color = new Vector3D(rand() / (double)RAND_MAX,
				 rand() / (double)RAND_MAX,
				 rand() / (double)RAND_MAX);
  Bleep *new_bleep;
  if (id == -1) {
    new_bleep = new Bleep(x, y, color);
  } else {
    new_bleep = new Bleep(x, y, color, id);
  }
  pthread_mutex_lock(&bleeps_mutex);
  bleeps.push_back(new_bleep);
  pthread_mutex_unlock(&bleeps_mutex);    
  if (send && g_connected) {
    SendAddBleep(new_bleep);
  }
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
	  Vector3D position(x, y, 0.0);
	  bool found_hit = false;
	  pthread_mutex_lock(&bleeps_mutex);    
	  vector<Bleep *>::iterator itr=bleeps.begin();
	  while(itr != bleeps.end()) {
	    Bleep *bleep = *itr;
	    Vector3D diff(bleep->position->x - position.x,
			  bleep->position->y - position.y,
			  bleep->position->z - position.z);
	    if (diff.magnitude() <= g_bleep_radius) {
	      found_hit = true;
	      break;
	    }
	    ++itr;
	  }
	  pthread_mutex_unlock(&bleeps_mutex);
	  if (found_hit) {
	    Bleep *bleep = *itr;
	    g_dragging_bleep = bleep;
	  } else {
	    AddBleep(x, y, true, -1);
	  }   
        }
        else
        {
	  g_dragging_bleep = NULL;
        }
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        // when right mouse button down
        if( state == GLUT_DOWN )
        {
	  Vector3D *position = new Vector3D(x, y, 0.0);
	  bool found_hit = false;
	  pthread_mutex_lock(&bleeps_mutex);    
	  vector<Bleep *>::iterator itr=bleeps.begin();
	  while(itr != bleeps.end()) {
	    Bleep *bleep = *itr;	    
	    Vector3D diff(bleep->position->x - position->x,
			  bleep->position->y - position->y,
			  bleep->position->z - position->z);
	    if (diff.magnitude() <= g_bleep_radius) {
	      found_hit = true;
	      break;
	    }
	    ++itr;
	  }
	  pthread_mutex_unlock(&bleeps_mutex);
	  if (found_hit) {
	    if (g_connected) {
	      SendEraseBleep((*itr)->id);
	    }
	    bleeps.erase(itr);
	  }
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

void mouseMotionFunc(int x,int y)
{
  if (g_dragging_bleep) {
    g_dragging_bleep->SetPosition(x, y);
    if (g_connected) {
      SendMoveBleep(g_dragging_bleep->id, x, y);
    }
  }
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

  static GLfloat time_line_x = 0;

  glPushMatrix();
  
  glColor3f(0.5, 0.3, 0.9);

  glBegin(GL_LINES);

  glVertex3f(time_line_x, 0, 0);
  glVertex3f(time_line_x, g_height, 0);

  glEnd();

  glPopMatrix();
  
  time_line_x += 2.0;
  if (time_line_x > g_width) {
    g_loop_count++;
    time_line_x -= g_width;
  }

  glEnable(GL_LIGHTING);

  pthread_mutex_lock(&bleeps_mutex);
  vector<Bleep *>::iterator itr=bleeps.begin();
  while(itr != bleeps.end()) {
    Bleep *bleep = *itr;
    bleep->update_color();

    if (time_line_x > bleep->position->x - g_bleep_radius &&
	time_line_x < bleep->position->x + g_bleep_radius) {
      bleep->play(g_loop_count);
    }

    glPushMatrix();
    
    glTranslatef(bleep->position->x,
		 bleep->position->y,
		 1.0);
    glColor3f(bleep->red->value,
	      bleep->green->value,
	      bleep->blue->value);
    glutSolidSphere(g_bleep_radius, 16, 16);
    
    glPopMatrix();

    ++itr;
  }
  pthread_mutex_unlock(&bleeps_mutex);

  glDisable(GL_LIGHTING);

  glColor3f(1, 1, 1);
  renderSpacedBitmapString(10, 10, 5, GLUT_BITMAP_8_BY_13, g_hostname);

  if (g_connect_mode) {
    renderSpacedBitmapString(g_width / 2 - 150, g_height / 2, 5, GLUT_BITMAP_8_BY_13, "connect:");
    renderSpacedBitmapString(g_width / 2 - 50, g_height / 2, 5, GLUT_BITMAP_8_BY_13, g_other_address);

  }
	  
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

    pthread_mutex_lock(&bleeps_mutex);
    vector<Bleep *>::iterator itr=bleeps.begin();
    while(itr != bleeps.end()) {
      Bleep *bleep = *itr;

      output_sample += bleep->ugen->GetSample(tick_count) * 0.1;

      ++itr;
    }
    pthread_mutex_unlock(&bleeps_mutex);
    
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

class ExamplePacketListener : public osc::OscPacketListener {
protected:
  
  virtual void ProcessMessage( const osc::ReceivedMessage& m, 
			       const IpEndpointName& remoteEndpoint )
  {
    
    try{
      
      if( strcmp( m.AddressPattern(), "/connect" ) == 0 ){
	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	const char *hostname;
	args >> hostname >> osc::EndMessage;
        
	memcpy(g_other_address, hostname, 255);

	g_other_host_and_port[0] = strtok(g_other_address, ":");
	g_other_host_and_port[1] = strtok(NULL, " .,-");

	g_connected = true;
      }
      else if( strcmp( m.AddressPattern(), "/add" ) == 0 ){
	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	osc::int32 id;
	float x;
	float y;
	args >> id >> x >> y >> osc::EndMessage;
	AddBleep(x, y, false, id);
      }
      else if( strcmp( m.AddressPattern(), "/erase" ) == 0 ){
	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	osc::int32 id;
	args >> id >> osc::EndMessage;
  
	bool found_hit = false;
	pthread_mutex_lock(&bleeps_mutex);
	vector<Bleep *>::iterator itr=bleeps.begin();
	while(itr != bleeps.end()) {
	  Bleep *bleep = *itr;
	  if (bleep->id == id) {
	    found_hit = true;
	    break;
	  }
	  ++itr;
	}
	if (found_hit) {
	  bleeps.erase(itr);
	}
	pthread_mutex_unlock(&bleeps_mutex);

      } else if( strcmp( m.AddressPattern(), "/move" ) == 0 ){
	// example #1 -- argument stream interface
	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	osc::int32 id;
	float x;
	float y;
	args >> id >> x >> y >> osc::EndMessage;
  
	bool found_hit = false;
	pthread_mutex_lock(&bleeps_mutex);
	vector<Bleep *>::iterator itr=bleeps.begin();
	while(itr != bleeps.end()) {
	  Bleep *bleep = *itr;
	  if (bleep->id == id) {
	    found_hit = true;
	    break;
	  }
	  ++itr;
	}
	if (found_hit) {
	  Bleep *bleep = *itr;
	  bleep->SetPosition(x, y);
	}
	pthread_mutex_unlock(&bleeps_mutex);

      }
    }catch( osc::Exception& e ){
      // any parsing errors such as unexpected argument types, or 
      // missing arguments get thrown as exceptions.
      std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
      
    }
  }
};

void *startThread( void* _socket )
{
  UdpListeningReceiveSocket* socket = (UdpListeningReceiveSocket*)_socket;
  socket->RunUntilSigInt();
  return NULL;
}

int main(int argc, char* argv[])
{
  pthread_mutex_init(&bleeps_mutex, NULL);

  // initialize GLUT
  glutInit( &argc, argv );
  // double buffer, use rgb color, enable depth buffer
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  // initialize the window size
  glutInitWindowSize( g_width, g_height );
  // set the window postion
  glutInitWindowPosition( 100, 100 );
  // create the window
  glutCreateWindow( "Webleep" );

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
  // set the mouse function - called on mouse stuff
  glutMouseFunc(mouseFunc);  
  glutMotionFunc(mouseMotionFunc);

  GLfloat light0_position[] = { 0.0, 0.0, -1.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glEnable(GL_LIGHT0);
  glEnable ( GL_LIGHTING ) ;

  glEnable(GL_COLOR_MATERIAL);

  try {
    SetUpAudio();
  } catch (RtError &e) {
    e.printMessage();
  }

  ExamplePacketListener listener;
  UdpListeningReceiveSocket *s;
  bool success = false;
  while (!success) {
    try {
      s = new UdpListeningReceiveSocket(IpEndpointName( IpEndpointName::ANY_ADDRESS, g_port ),
					&listener );
      success = true;
    } catch ( std::runtime_error &e ) {
      g_port += 1;
    }
  }

  char local_hostname[255];
  gethostname(local_hostname, 255);

  IpEndpointName local_ip_endpoint( local_hostname, g_port );
  char local_ip_address[ IpEndpointName::ADDRESS_STRING_LENGTH ];
  local_ip_endpoint.AddressAsString( local_ip_address );

  sprintf(g_hostname, "%s:%d", local_ip_address, g_port);

  pthread_create(&oscpack_thread, NULL, startThread, (void *)s);

  // let GLUT handle the current thread from here
  glutMainLoop();

  pthread_cancel(oscpack_thread);

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
