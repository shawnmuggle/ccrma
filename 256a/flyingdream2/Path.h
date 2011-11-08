/*
 *  Path.h
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "vector3d.h"

#include "fluidsynth.h"

#include "PitchMapper.h"

#include <vector>
#include <pthread.h>


class Path
{
public:
    Path(int instrument_number, Vector3D *color);
    ~Path();
    void AddPoint(Vector3D *point);
    void Update(Vector3D *player_position);
    void Play(fluid_synth_t *synth, PitchMapper *mapper);
    void Render();
    bool Done();
    void FinishedDrawing();
private:
    std::vector<Vector3D *> *points;
    int playhead_index;
    int instrument_number, pitch, prev_pitch;
    int play_count, lifetime_plays, finished_drawing;
    bool play_something;
    time_t time_finished_drawing;
    int volume;
    Vector3D color;
    pthread_mutex_t points_mutex;
};