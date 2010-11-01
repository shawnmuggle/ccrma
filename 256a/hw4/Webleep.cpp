#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/ip/IpEndpointName.h"

#ifdef __MACOSX_CORE__
  // note: for mac only
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;


#define ADDRESS "127.0.0.1"
#define PORT 7000

#define OUTPUT_BUFFER_SIZE 1024

#define IP_MTU_SIZE 1536

// width and height of the window
GLsizei g_width = 800;
GLsizei g_height = 600;

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
    case 'r':
      //cout << "HELLO R" << endl;
      break;
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
  /*
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
  */

  // clear the color and depth buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  // flush!
  glFlush( );
  // swap the double buffer
  glutSwapBuffers( );
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
      
      if( strcmp( m.AddressPattern(), "/play" ) == 0 ){
	// example #1 -- argument stream interface
	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	osc::int32 a1;
	args >> a1 >> osc::EndMessage;
        
	std::cout << "received '/play' message with argument: "
		  << a1 << "\n";
	
      }
    }catch( osc::Exception& e ){
      // any parsing errors such as unexpected argument types, or 
      // missing arguments get thrown as exceptions.
      std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
    }
  }
};

int main(int argc, char* argv[])
{
  if (argc == 1) {
    cout << "SENDING" << endl;

    const char *hostName = "localhost";

    IpEndpointName host( hostName, PORT );
    
    char hostIpAddress[ IpEndpointName::ADDRESS_STRING_LENGTH ];
    host.AddressAsString( hostIpAddress );
    
    std::cout << "sending test messages to " << hostName 
	      << " (" << hostIpAddress << ") on port " << PORT << "...\n";

    char buffer[IP_MTU_SIZE];
    osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
    UdpTransmitSocket socket( host );

    p.Clear();
    p << osc::BeginMessage( "/play" ) << 23 << osc::EndMessage;
    socket.Send( p.Data(), p.Size() );

  } else {
    cout << "LISTENING" << endl;
    
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
    

    ExamplePacketListener listener;
    UdpListeningReceiveSocket s(
				IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
				&listener );
    
    std::cout << "press ctrl-c to end\n";
    
    // let GLUT handle the current thread from here
    glutMainLoop();
  
    //s.RunUntilSigInt();
  }
  return 0;
}

