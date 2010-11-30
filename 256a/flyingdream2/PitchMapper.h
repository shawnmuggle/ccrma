/*
 *  PitchMapper.h
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PITCHMAPPER_H
#define PITCHMAPPER_H

#include <vector>

class PitchMapper
{
public:
    PitchMapper(int root_pitch, std::vector<int> *scale_intervals);
    ~PitchMapper();
    int Map(int pitch);
    
private:
    int root_pitch;
    std::vector<int> *scale_intervals;
};

#endif