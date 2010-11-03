#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/ip/IpEndpointName.h"

#ifdef __MAC__
  // note: for mac only
  #include <GLUT/glut.h>
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

using namespace std;


#define ADDRESS "127.0.0.1"

#define OUTPUT_BUFFER_SIZE 1024

#define IP_MTU_SIZE 1536

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
  Bleep(Vector3D *position, Vector3D *color) : 
    position(position),
    last_loop_seen(-1)
  {
    sine = new Sine(20 + g_height - position->y);
    env = new AREnvelope(5, 10000, 1.0);
    env->GetAudioFrom(sine);
    ugen = env;

    red = new Vector3D(color->x, color->x, 0.1);
    green = new Vector3D(color->y, color->y, 0.1);
    blue = new Vector3D(color->z, color->z, 0.1);
    delete color;
  }
  ~Bleep() { delete position; delete red; delete green; delete blue; }
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
    sine->SetFrequency(20 + g_height - y);
  }

public:
  Vector3D *red;
  Vector3D *green;
  Vector3D *blue;
  Vector3D *position;
  UGen *ugen;

private:
  int last_loop_seen;
  Sine *sine;
  AREnvelope *env;
};

vector<Bleep *> bleeps;
pthread_mutex_t bleeps_mutex;
Bleep *g_dragging_bleep = NULL;

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
  cout << "CONNECTING TO: " << g_other_host_and_port[0] << " : " << g_other_host_and_port[1] << endl;
  
  int port = atoi(g_other_host_and_port[1]);
  IpEndpointName host( g_other_host_and_port[0], port );
  
  char hostIpAddress[ IpEndpointName::ADDRESS_STRING_LENGTH ];
  host.AddressAsString( hostIpAddress );
  
  std::cout << "sending /connect to " << g_other_host_and_port[0] 
	    << " (" << hostIpAddress << ") on port " << port << "...\n";
  
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
    //cout << g_other_address << endl;
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

void SendAddBleep(Vector3D *position)
{
  cout << "SENDING ADD BLEEP" << endl;
  int port = atoi(g_other_host_and_port[1]);
  IpEndpointName host( g_other_host_and_port[0], port );
  
  cout << "YUP" << endl;
  char buffer[IP_MTU_SIZE];
  osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
  UdpTransmitSocket socket( host );
  
  cout << "OYES" << endl;
  p.Clear();
  p << osc::BeginMessage( "/add" ) << position->x << position->y << osc::EndMessage;
  socket.Send( p.Data(), p.Size() );
}

void AddBleep(Vector3D *position, bool send)
{
  cout << "ADDING BLEEP" << endl;
  Vector3D *color = new Vector3D(rand() / (double)RAND_MAX,
				 rand() / (double)RAND_MAX,
				 rand() / (double)RAND_MAX);
  cout << "1" << endl;
  pthread_mutex_lock(&bleeps_mutex);    
  cout << "2" << endl;
  bleeps.push_back(new Bleep(position, color));
  cout << "3" << endl;
  pthread_mutex_unlock(&bleeps_mutex);    
  cout << "4" << endl;
  if (send && g_connected) {
    cout << "5" << endl;
    SendAddBleep(position);
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
	    Bleep *bleep = *itr;
	    g_dragging_bleep = bleep;
	  } else {
	    AddBleep(position, true);
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
	  if (found_hit) {
	    bleeps.erase(itr);
	  }
	  pthread_mutex_unlock(&bleeps_mutex);
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
  //cout << "ACTUAL DELTA: " << XGfx::delta() << endl;
  time_accumulator += XGfx::delta();
  if (time_accumulator < 1.0 / 60.0) {
    //cout << "HERP" << endl;
    return;
  }
  //cout << "RENDERTIME at time: " << time_accumulator << endl;
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
    cout << "Received " << m.AddressPattern() << endl;
    
    try{
      // example of parsing single messages. osc::OsckPacketListener
      // handles the bundle traversal.
      
      if( strcmp( m.AddressPattern(), "/connect" ) == 0 ){
	// example #1 -- argument stream interface
	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	const char *hostname;
	args >> hostname >> osc::EndMessage;
        
	std::cout << "received '/connect' message with argument: "
		  << hostname << "\n";
	
	memcpy(g_other_address, hostname, 255);

	cout << g_connected << endl;

	if (!g_connected) {
	  cout << "Trying to back-connect" << endl;
	  TryToConnectToHost();
	} 
      }
      else if( strcmp( m.AddressPattern(), "/add" ) == 0 ){
	// example #1 -- argument stream interface
	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	float x;
	float y;
	args >> x >> y >> osc::EndMessage;
        
	std::cout << "received '/add' message with argument: "
		  << x << ", " <<  y << "\n";
	
	AddBleep(new Vector3D(x, y, 0), false);
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
  cout << "CONNECTED? " << g_connected << endl;

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
  glutCreateWindow( "VisualSine" );

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

  cout << "LISTENING" << endl;
  
  ExamplePacketListener listener;
  UdpListeningReceiveSocket *s;
  bool success = false;
  while (!success) {
    try {
      cout << "Trying to listen on port " << g_port << endl;
      s = new UdpListeningReceiveSocket(IpEndpointName( IpEndpointName::ANY_ADDRESS, g_port ),
					&listener );
      success = true;
    } catch ( std::runtime_error &e ) {
      g_port += 1;
    }
  }

  char local_hostname[255];
  gethostname(local_hostname, 255);

  cout << "hostname: " << local_hostname << endl;

  IpEndpointName local_ip_endpoint( local_hostname, g_port );
  char local_ip_address[ IpEndpointName::ADDRESS_STRING_LENGTH ];
  local_ip_endpoint.AddressAsString( local_ip_address );

  cout << "MY IP IS " << local_ip_address << endl;
  
  sprintf(g_hostname, "%s:%d", local_ip_address, g_port);
  cout << "host and port: " << g_hostname << endl;

  cout << "CONNECTED? " << g_connected << endl;  

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
