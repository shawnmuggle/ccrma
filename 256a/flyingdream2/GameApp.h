/*
 *  GameApp.h
 *  SDL Test
 *
 *  Created by Mark Szymczyk on 5/1/06.
 *  Copyright 2006 Me and Mark Publishing. All rights reserved.
 *
 */

#include "SDL.h"
#include "SDL_OpenGL.h"
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>

#include <fluidsynth.h>

//#include "fmod.hpp"
//#include "fmod_errors.h"

#include "RtAudio.h"
#include "RtError.h"

#include "SineWave.h"
#include "NRev.h"
#include "Noise.h"

#include "Entity.h"
#include "Environment.h"
#include "Path.h"
#include "PitchMapper.h"

#include <vector>
#include <deque>

struct synths {
    stk::Noise *noise;
};

class GameApp
{
protected:
    SDL_TimerID timer;
    bool done;
    
public:
    // Constructor and destructor
    GameApp (void);
    virtual ~GameApp(void);
    
    // Initialization functions
    void InitApp(void);
    void InitializeSDL(void);
    void InstallTimer(void);
    static Uint32 GameLoopTimer(Uint32 interval, void* param);
    
    // Cleanup functions
    void Cleanup(void);
    
    // Event-related functions
    void EventLoop(void);
    void HandleUserEvents(SDL_Event* event);
    
    // Game related functions
    void GameLoop(void);
    void RenderFrame(void);
    
    void AddCuboid(float x, float y, float z);

private:
    float g_pitch, g_yaw, g_roll;
    Vector3D g_position;
    Vector3D g_velocity;
    bool g_forward_held, g_backward_held, g_space_held;
    
    float g_width, g_height;
    
    //Skybox *g_skybox;
    //Cuboid *g_ocean;
    
    Environment *g_environment;
    
    std::vector<Cuboid *> *g_cuboids;
    bool g_tracking;
    //std::vector<std::vector<Vector3D *>*> g_paths;
    std::deque<Path *> g_paths;
    Vector3D g_gravity;
    bool g_gravity_on;
    PitchMapper *g_pitch_mapper;
    
    int instrument_id;
    Vector3D *instrument_color;
    
//    fluid_settings_t* settings;
//    fluid_synth_t* synth;
//    fluid_audio_driver_t* adriver;
    
    //FMOD::System           *system;
    
    RtAudio dac;
    
    //stk::SineWave *sine;
    //stk::NRev *reverb;
    //stk::Noise *noise;
};

typedef GameApp* GameAppPtr;
typedef GameApp** GameAppHandle;

const int RUN_GAME_LOOP = 1;