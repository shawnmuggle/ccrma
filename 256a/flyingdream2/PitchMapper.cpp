/*
 *  PitchMapper.cpp
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "PitchMapper.h"

PitchMapper::PitchMapper(int root_pitch, std::vector<int> *scale_intervals) :
root_pitch(root_pitch),
scale_intervals(scale_intervals)
{ }

PitchMapper::~PitchMapper()
{
    delete scale_intervals;
}

int PitchMapper::Map(int pitch)
{
    int diatonic_pitch = root_pitch;
    int prev_pitch;

    if (diatonic_pitch < pitch) {
        std::vector<int>::iterator scale_itr = scale_intervals->begin();
        while (diatonic_pitch <= pitch) {
            prev_pitch = diatonic_pitch;
            diatonic_pitch += *scale_itr;
            
            if (diatonic_pitch > pitch) {
                // round down
                return prev_pitch;
            }
            
            scale_itr++;
            if (scale_itr == scale_intervals->end()) {
                scale_itr = scale_intervals->begin();
            }
        }
    } else if (diatonic_pitch > pitch) {
        std::vector<int>::reverse_iterator scale_itr = scale_intervals->rbegin();
        while (diatonic_pitch >= pitch) {
            prev_pitch = diatonic_pitch;
            diatonic_pitch -= *scale_itr;
            
            if (diatonic_pitch < pitch) {
                // round down
                return prev_pitch;
            }
            
            scale_itr++;
            if (scale_itr == scale_intervals->rend()) {
                scale_itr = scale_intervals->rbegin();
            }
        }
    }
    
    // else diatonic_pitch == pitch
    return pitch;
}