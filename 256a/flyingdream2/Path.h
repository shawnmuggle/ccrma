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

class Path
{
public:
    Path();
    ~Path();
    void AddPoint(Vector3D *point);
    void Update();
    void Play(fluid_synth_t *synth, PitchMapper *mapper);
    void Render();
    bool Done();
    void FinishedDrawing();
private:
    std::vector<Vector3D *> *points;
    int playhead_index;
    int instrument_number, pitch, prev_pitch;
    int play_count, lifetime_plays, finished_drawing;
};