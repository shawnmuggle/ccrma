/*
 *  Path.cpp
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Path.h"

Path::Path() :
playhead_index(0),
//instrument_number(rand() / (double)RAND_MAX * 128),
instrument_number(106),
pitch(0),
prev_pitch(0),
play_count(0),
lifetime_plays(10),
finished_drawing(false),
points(new std::vector<Vector3D *>())
{
    printf("instrument: %d\n", instrument_number);
}

Path::~Path()
{
    delete points;
}

void Path::AddPoint(Vector3D *point)
{
    points->push_back(point);
}

void Path::Update()
{
    playhead_index += 2;
    if (playhead_index >= points->size()) {
        playhead_index = 0;
        pitch = -1;
        if (finished_drawing) {
            ++play_count;
        }
    }
}

bool Path::Done()
{
    return play_count >= lifetime_plays;
}

void Path::FinishedDrawing()
{
    finished_drawing = true;
}

void Path::Play(fluid_synth_t *synth, PitchMapper *mapper)
{
    prev_pitch = pitch;
    pitch = mapper->Map(128 - (points->at(playhead_index)->y + 1000) / 2000 * 128);
    if (prev_pitch != pitch) {
        fluid_synth_noteoff(synth, instrument_number, prev_pitch);
        fluid_synth_noteon(synth, instrument_number, pitch, 80 - 80 * (play_count / (float)lifetime_plays));
    }
}

void Path::Render()
{
    std::vector<Vector3D *>::iterator point_itr=points->begin();
    Vector3D *point, *prev_point;
    int point_index = 0, num_segments = 16;
    float width, prev_width = 0, rad = 0;

    // increment so we start at the second point
    ++point_itr;
    ++point_index;
    
    while(point_itr != points->end()) {
        
        point = *point_itr;
        prev_point = *(point_itr - 1);
                
        width = 1;
        
        GLfloat mat_amb_diff[] = { 0.8, 0.5, 0.2, 1 - play_count / (float)lifetime_plays };
        float dist = abs(point_index - playhead_index);
        if (dist < 10) {
            width = pow(10 - dist, 1.01);
            mat_amb_diff[0] = 0.8 + 0.2 * (1 - dist / 10);
            mat_amb_diff[1] = 0.5 + 0.5 * (1 - dist / 10);
            mat_amb_diff[2] = 0.2 + 0.8 * (1 - dist / 10);
        }

        if (point_itr + 1 == points->end()) {
            width = 0;
        }
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= num_segments; i++) {
            rad = (i / 16.0) * 2 * M_PI;
            glNormal3f(cos(rad), sin(rad), 0.0f);
            glVertex3f(-prev_point->x + prev_width * cos(rad), -prev_point->y + prev_width * sin(rad), -prev_point->z);
            glVertex3f(-point->x + width * cos(rad), -point->y + width * sin(rad), -point->z);
        }
        glEnd();
        prev_width = width;
        
        ++point_itr;
        ++point_index;
    }    
}