#include "RtAudio.h"
#ifdef __MACOSX_CORE__
  // note: for mac only
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

#define SAMPLE double

//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void idleFunc( );
void displayFunc( );
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );


class Synth {
  RtAudio audio;
  RtAudio::StreamParameters output_params;
  RtAudio::StreamParameters input_params;

  static int AudioCallback(void *output_buffer, void *input_buffer, unsigned int n_buffer_frames,
			   double stream_time, RtAudioStreamStatus status, void *data);
 public:
  void SetUpAudio();
};


