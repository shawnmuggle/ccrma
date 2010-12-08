/*
 *  GameApp.cpp
 *  SDL Test
 *
 *  Created by Mark Szymczyk on 5/1/06.
 *  Copyright 2006 Me and Mark Publishing. All rights reserved.
 *
 */

// TODO:
// better clouds (move, transparent, flatter & more bumpy
// redo sky & ocean drawing to draw everything correctly (will need environment-specific rendering code)
// Bound position within skybox
// better lighting (as if there's a sun?
// draw tubes correctly (make them correct in all directions)
// make tubes look neat (with "random" (aka wind wisps from wii zelda) lines around it?)
// bigger, more interesting environment (planet? sun/moon?)
// improve wind sound
// spatialize sound based on where you are relative to source
// louder
// blocky "pixels" of randomized color around a base, ˆ la minecraft
// remap pitch to increased size of world
// muck with sound design
// make paths visible from above
// fix crash when start path while moving backwards (?!?) (width == -1, hmm)
// make clouds white again (emit light?)
// fuck around with lighting so it's brighter and doesn't do the "pits or hills" visual illusion on the ground somehow

// IDEAS:
// environment makes music, which has a rhythm represented visually (waves, wind, etc)
// environmental music is effected by your actions (instrumentation, rhythm, tempo, intensity etc)
// occasionally a black portal will open up into the world and spit out bad lines, and you need to follow them with music paths to "erase" them

#include "BiQuad.h"
#include "Noise.h"
#include "SineWave.h"
#include "NRev.h"

#include "GameApp.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

const float PI       =  3.14159265358979323846f;
#define	DEGTORAD(x)	( ((x) * PI) / 180.0 )
#define	RADTODEG(x)	( ((x) * 180.0) / PI )

#define SAMPLE float

static fluid_settings_t* settings;
static fluid_synth_t* synth;

stk::Noise noise;
stk::NRev reverb;
stk::BiQuad biquad;

int AudioCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                  double streamTime, RtAudioStreamStatus status, void *userData )
{
    SAMPLE *buffer = (SAMPLE *) outputBuffer;    
    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
    
    fluid_synth_write_float(synth, nBufferFrames, buffer, 0, 2, buffer, 1, 2);
    
    stk::StkFloat l_output_sample, r_output_sample, noise_sample;
    for ( unsigned int i=0; i<nBufferFrames; i++ ) {
        //output_sample = noise.tick();

        l_output_sample = buffer[i*2];
        l_output_sample = reverb.tick(l_output_sample, 0);

        r_output_sample = buffer[i*2+1];
        r_output_sample = reverb.lastFrame()[1];//reverb.tick(r_output_sample, 0);

        noise_sample = biquad.tick(noise.tick()) * 0.1;
        
        l_output_sample += noise_sample;
        r_output_sample += noise_sample;
        
        buffer[i*2] = l_output_sample;  // single-sample computations
        buffer[i*2+1] = r_output_sample;  // single-sample computations
    }

    
    return 0;
}


// Constructor
GameApp::GameApp(void)
{
    stk::Stk::setSampleRate( 44100.0 );

    biquad.setResonance( 20.0, 0.99, true );
    biquad.setEqualGainZeroes();
    
    biquad.setGain(0);
    
    g_width = 1440;
    g_height = 855;
    g_pitch = g_yaw = g_roll = 0;
    g_tracking = false;
    done = false;
    g_forward_held = false;
    g_backward_held = false;
    
    g_gravity = Vector3D(0, 0.2, 0);
    g_gravity_on = true;
    
    g_cuboids = new vector<Cuboid *>();
    
    vector<int> *scale_degrees = new vector<int>();

    /*
    // Major scale
    scale_degrees->push_back(2);
     scale_degrees->push_back(2);
    scale_degrees->push_back(1);
    scale_degrees->push_back(2);
    scale_degrees->push_back(2);
    scale_degrees->push_back(2);
    scale_degrees->push_back(1);
     */
    
    // Major Pentatonic
    scale_degrees->push_back(3);
    scale_degrees->push_back(2);
    scale_degrees->push_back(2);
    scale_degrees->push_back(3);
    scale_degrees->push_back(2);
    
    /*
    // Octave
    scale_degrees->push_back(12);
    */
    
    g_pitch_mapper = new PitchMapper(60, scale_degrees);
    
    settings = new_fluid_settings();
    
    fluid_settings_setint(settings, "synth.midi-channels", 128);
    synth = new_fluid_synth(settings);
    
    //fluid_settings_setstr(settings, "audio.driver", "coreaudio");
    //adriver = new_fluid_audio_driver(settings, synth);
    
    fluid_synth_sfload(synth, "/Users/mrotondo/Downloads/TimGM6mb.sf2", 1);
    
    if ( dac.getDeviceCount() < 1 ) {
        std::cout << "\nNo audio devices found!\n";
        exit( 0 );
    }
    
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256; // 256 sample frames
    try {
        dac.openStream(&parameters, 
                       NULL, 
                       RTAUDIO_FLOAT32,
                       sampleRate, 
                       &bufferFrames, 
                       &AudioCallback, 
                       NULL);
                       //(void *)&s );
        dac.startStream();
    }
    catch ( RtError& e ) {
        e.printMessage();
        exit( 0 );
    }
}

// Destructor
GameApp::~GameApp(void)
{
    /*
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
    
    try {
        // Stop the stream
        dac.stopStream();
    }
    catch (RtError& e) {
        e.printMessage();
    }
    
    if ( dac.isStreamOpen() ) dac.closeStream();
    */
}

void GameApp::AddCuboid(float x, float y, float z)
{
    Vector3D *color = new Vector3D(rand() / (double)RAND_MAX,
                                   rand() / (double)RAND_MAX,
                                   rand() / (double)RAND_MAX);
    Cuboid *new_cuboid = new Cuboid(new Vector3D(x, y, z), color, 1.0, 1.0);
    g_cuboids->push_back(new_cuboid);
}

// Initialization functions
void GameApp::InitApp(void)
{
    for (int i = 0; i < 1000; i++) {
        AddCuboid(1000 - 2000.0 * rand() / RAND_MAX, 
                  1000 - 2000.0 * rand() / RAND_MAX, 
                  1000 - 2000.0 * rand() / RAND_MAX);
    }
    
    InitializeSDL();

    g_environment = new Environment();

    InstallTimer();
    SDL_ShowCursor(SDL_DISABLE);
    
}

void GameApp::InitializeSDL(void)
{
    int error;
    SDL_Surface* drawContext;
    
    error = SDL_Init(SDL_INIT_EVERYTHING);
    
    // Create a double-buffered draw context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    
    
        
    Uint32 flags;
    flags = SDL_OPENGL; // | SDL_FULLSCREEN;
    drawContext = SDL_SetVideoMode(g_width, g_height, 0, flags);
    

    glViewport( 0, 0, ( GLsizei )g_width, ( GLsizei )g_height );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, g_width/g_height, 0.1, 8000);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_CULL_FACE);
    
    glHint(GL_POLYGON_SMOOTH, GL_NICEST);
    
    
    
    
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    
    //glEnable(GL_COLOR_MATERIAL);
    //GLfloat LightAmbient[]= { 0.6f, 0.6f, 0.6f, 1.0f }; 				// Ambient Light Values ( NEW )
    //glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);				// Setup The Ambient Light
    GLfloat LightDiffuse[]= { 0.5f, 0.5f, 0.5f, 0.5f };				 // Diffuse Light Values ( NEW )
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);				// Setup The Diffuse Light
    glEnable(GL_LIGHT1);							// Enable Light One
    
    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    
    GLfloat fogColor[4]= {127/255.0, 178/255.0, 240/255.0, 1.0f};		// Fog Color
    glFogi(GL_FOG_MODE, GL_LINEAR);		// Fog Mode
    glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
    glFogf(GL_FOG_DENSITY, 1.0f);				// How Dense Will The Fog Be
    glHint(GL_FOG_HINT, GL_NICEST);			// Fog Hint Value
    glFogf(GL_FOG_START, 500.0f);				// Fog Start Depth
    glFogf(GL_FOG_END, 2000.0f);				// Fog End Depth
//    glEnable(GL_FOG);					// Enables GL_FOG
}

void GameApp::InstallTimer(void)
{
    timer = SDL_AddTimer(30, GameLoopTimer, this);
}

Uint32 GameApp::GameLoopTimer(Uint32 interval, void* param)
{
    // Create a user event to call the game loop.
    SDL_Event event;
    
    event.type = SDL_USEREVENT;
    event.user.code = RUN_GAME_LOOP;
    event.user.data1 = 0;
    event.user.data2 = 0;
    
    SDL_PushEvent(&event);
    
    return interval;
}


// Cleanup functions
void GameApp::Cleanup(void)
{
    SDL_bool success;
    success = SDL_RemoveTimer(timer);
    
    SDL_Quit();
}


// Event-related functions
void GameApp::EventLoop(void)
{
    SDL_Event event;
    
    vector<Cuboid *>::iterator itr;
    Cuboid *cuboid;

    while((!done) && (SDL_WaitEvent(&event))) {
        switch(event.type) {
            case SDL_USEREVENT:
                HandleUserEvents(&event);
                break;
                
            case SDL_MOUSEMOTION:
                if (abs(event.motion.xrel) > 50 || abs(event.motion.yrel) > 50) {
                    break;
                }
                g_yaw += event.motion.xrel * 0.3;                
                g_pitch += event.motion.yrel * 0.3;
                
                if (g_pitch > 90) g_pitch = 90;
                if (g_pitch < -90) g_pitch = -90;
                
                if (event.motion.x < 10 || event.motion.x > g_width - 10 ||
                    event.motion.y < 10 || event.motion.y > g_height - 10) {
                    SDL_WarpMouse(g_width / 2, g_height / 2);
                    SDL_ShowCursor(SDL_DISABLE);
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        done = true;
                        break;
                    default:
                        break;
                    case SDLK_SPACE:
                        itr=g_cuboids->begin();                
                        while(itr != g_cuboids->end()) {
                            cuboid = *itr;                            
                            cuboid->Flash();
                            ++itr;
                        }
                        g_tracking = true;
                        g_paths.push_back(new Path());
                        break;
                    case SDLK_w:
                        g_forward_held = true;
                        break;
                    case SDLK_s:
                        g_backward_held = true;
                        break;
                    case SDLK_g:
                        g_gravity_on = g_gravity_on ? false : true;
                        break;
                    case SDLK_f:
                        g_gravity_on = false;
                        g_velocity = Vector3D(0, 0, 0);
                        break;
                    case SDLK_r:
                        g_position = Vector3D(0, 0, 0);
                        break;
                    case SDLK_d:
                        fluid_synth_noteon(synth, 0, 60, 100);
                        break;

                        
                }
                break;
                
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        g_forward_held = false;
                        break;
                    case SDLK_s:
                        g_backward_held = false;
                        break;
                    case SDLK_SPACE:
                        itr=g_cuboids->begin();                
                        while(itr != g_cuboids->end()) {
                            cuboid = *itr;                            
                            cuboid->Flash();
                            ++itr;
                        }
                        g_tracking = false;
                        g_paths.back()->FinishedDrawing();
                        break;
                    case SDLK_d:
                        fluid_synth_noteoff(synth, 0, 60);
                        break;
                }
                break;

            
            case SDL_QUIT:
                done = true;
                break;
                
            default:
                break;
        }   // End switch
            
    }   // End while
        
}

void GameApp::HandleUserEvents(SDL_Event* event)
{
    switch (event->user.code) {
        case RUN_GAME_LOOP:
            GameLoop();
            break;
            
        default:
            break;
    }
}


// Game related functions
void GameApp::GameLoop(void)
{
    Vector3D force;
    static float movement_scale = 0.2;
    float rad_pitch, rad_yaw;
    
    if (g_forward_held) {
        rad_pitch = DEGTORAD(g_pitch + 90);
        rad_yaw = DEGTORAD(g_yaw);
        
        force = Vector3D(-movement_scale * sin(rad_pitch) * sin(rad_yaw),
                         -movement_scale * cos(rad_pitch),
                         movement_scale * sin(rad_pitch) * cos(rad_yaw));
        g_velocity += force;
        
        if (g_tracking) {
            g_paths.back()->AddPoint(new Vector3D(g_position));
        }
    }
    if (g_backward_held) {
        rad_pitch = DEGTORAD(g_pitch + 90);
        rad_yaw = DEGTORAD(g_yaw);
        
        force = Vector3D(movement_scale * sin(rad_pitch) * sin(rad_yaw),
                         movement_scale * cos(rad_pitch),
                         -movement_scale * sin(rad_pitch) * cos(rad_yaw));
        g_velocity += force;
    }
    
    // gravity
    if (g_gravity_on) {
        g_velocity += g_gravity;
    }
    // air resistance
    g_velocity *= 0.99;
    //movement
    g_position += g_velocity * 0.5;
    
    g_environment->BoundPosition(&g_position, &g_velocity, &g_gravity);
    
    if (g_tracking) {
        g_paths.back()->AddPoint(new Vector3D(g_position));
    }
    
    float speed = g_velocity.magnitude();
    // arbitrary max speed?
    float speed_scale = speed / 20;
    biquad.setGain(speed_scale);
    biquad.setResonance(20 + 300 * speed_scale, 0.99, true);
    
    
    // Play paths
    deque<Path *>::iterator path_itr=g_paths.begin();
    while(path_itr != g_paths.end()) {
        Path *path = *path_itr;
        path->Update();
        
        path->Play(synth, g_pitch_mapper);
        ++path_itr;
    }
    while (!g_paths.empty() && (*g_paths.begin())->Done()) {
        g_paths.pop_front();
    }
    
    RenderFrame();    
}

void GameApp::RenderFrame(void) 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();
    
    glRotatef(g_pitch, 1, 0, 0);		// multiply into matrix
    glRotatef(g_yaw, 0, 1, 0);
    glTranslatef(g_position.x, g_position.y, g_position.z);

    GLfloat light_position[] = { 100.0, 200.0, 300.0, 0.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    
    g_environment->Render();
    
    vector<Cuboid *>::iterator itr=g_cuboids->begin();
    while(itr != g_cuboids->end()) {
        Cuboid *cuboid = *itr;
        cuboid->Render();
        ++itr;
    }
    
    deque<Path *>::iterator path_itr=g_paths.begin();
    while(path_itr != g_paths.end()) {
        Path *path = *path_itr;
        
        path->Render();
        
        ++path_itr;
    }

    g_environment->RenderClouds();
    
    glPopMatrix();

    SDL_GL_SwapBuffers();
}
