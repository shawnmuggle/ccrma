/*
 *  Path.cpp
 *  SDLOpenGLIntro
 *
 *  Created by Michael Rotondo on 11/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Path.h"

Path::Path(int instrument_number, Vector3D *color) :
playhead_index(0),
//instrument_number(rand() / (double)RAND_MAX * 128),
instrument_number(instrument_number),
color(*color),
pitch(0),
prev_pitch(0),
play_count(0),
lifetime_plays(10),
finished_drawing(false),
points(new std::vector<Vector3D *>())
{
    printf("New path has rgb: %f, %f, %f\n", color->x, color->y, color->z);
    play_something = true;
    pthread_mutex_init(&points_mutex, NULL);
    printf("instrument: %d\n", instrument_number);
}

Path::~Path()
{
    delete points;
}

void Path::AddPoint(Vector3D *point)
{
    pthread_mutex_lock(&points_mutex);
    points->push_back(point);
    pthread_mutex_unlock(&points_mutex);
}

void Path::Update(Vector3D *player_position)
{
    if (!play_something && (rand() / (float)RAND_MAX) > 0.95) {
        play_something = true;
    }
    
    Vector3D *point;
    double min_dist = 9999999999, square_dist;
    pthread_mutex_lock(&points_mutex);
    std::vector<Vector3D *>::iterator point_itr=points->begin();
    while(point_itr != points->end() - 1) {
        point = *point_itr;
        
        square_dist = pow(point->x - player_position->x, 2) + pow(point->y - player_position->y, 2) + pow(point->z - player_position->z, 2);
        if (square_dist < min_dist) {
            min_dist = square_dist;
        }
        
        ++point_itr;
    }
    pthread_mutex_unlock(&points_mutex);
    volume = 0;
    if (min_dist < 200000) {
        volume = 127 * pow((1 - min_dist / 200000), 2);
    }
}

bool Path::Done()
{
    return play_count >= lifetime_plays;    
}

void Path::FinishedDrawing()
{
    finished_drawing = true;
    time_finished_drawing = time(NULL);
}

void Path::Play(fluid_synth_t *synth, PitchMapper *mapper)
{
    if (Done()) {
        return;
    }
    
    if (play_something) {
        prev_pitch = pitch;
        pitch = mapper->Map(30 + (rand() / (float)RAND_MAX) * 60);
        fluid_synth_noteoff(synth, instrument_number, prev_pitch);
        fluid_synth_noteon(synth, instrument_number, pitch, volume);
        play_something = false;
    }
}

void Path::Render()
{
    pthread_mutex_lock(&points_mutex);
    if (points->size() < 3) {
        pthread_mutex_unlock(&points_mutex);
        return;
    }
    pthread_mutex_unlock(&points_mutex);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    Vector3D *point, *prev_point, *next_point;
    int point_index = 0;
    float width, prev_width = 0;
    float percent_scale = 0;
    
    float z_diff, y_diff, x_diff, theta, theta_right, theta_left, point_x, point_y, point_z, prev_point_x, prev_point_y, prev_point_z;
    
    // TODO: Make path rendering handle vertical angles as well as horizontal ones
    pthread_mutex_lock(&points_mutex);
    std::vector<Vector3D *>::iterator point_itr=points->begin();
    point = *point_itr;
    prev_point_x = point->x;
    prev_point_y = point->y;
    prev_point_z = point->z;
    // increment so we start at the second point
    ++point_itr;
    ++point_index;
    
    glBegin(GL_TRIANGLE_STRIP);
    while(point_itr != points->end() - 1) {
        
        point = *point_itr;
        prev_point = *(point_itr - 1);
        next_point = *(point_itr + 1);
        
        z_diff = next_point->z - prev_point->z;
        y_diff = next_point->y - prev_point->y;
        x_diff = next_point->x - prev_point->x;
        theta = atan2(z_diff, x_diff);
        theta_left = theta + M_PI / 2;
        theta_right = theta - M_PI / 2;
        
        point_x = 0.0 * point->x + 1.0 * ((next_point->x + prev_point->x) / 2);
        point_y = 0.0 * point->y + 1.0 * ((next_point->y + prev_point->y) / 2);
        point_z = 0.0 * point->z + 1.0 * ((next_point->z + prev_point->z) / 2);
        
        percent_scale = (1 - abs(points->size() / 2 - point_index) / ((float)points->size()/2));

        width = percent_scale;

        glColor4f(color.x, color.y, color.z, (1 - play_count / (float)lifetime_plays) * percent_scale * 1.5);

        glBegin(GL_QUAD_STRIP);
        glVertex3f(-point_x + width * cos(theta_right), 
                   -point_y, 
                   -point_z + width * sin(theta_right));
        glVertex3f(-prev_point_x + width * cos(theta_right), 
                   -prev_point_y, 
                   -prev_point_z + width * sin(theta_right));
        
        glVertex3f(-point_x, 
                   -point_y + width, 
                   -point_z);
        glVertex3f(-prev_point_x, 
                   -prev_point_y + width, 
                   -prev_point_z);
        
        glVertex3f(-point_x + width * cos(theta_left), 
                   -point_y, 
                   -point_z + width * sin(theta_left));
        glVertex3f(-prev_point_x + width * cos(theta_left), 
                   -prev_point_y, 
                   -prev_point_z + width * sin(theta_left));
        
        
        glVertex3f(-point_x, 
                   -point_y - width, 
                   -point_z);
        glVertex3f(-prev_point_x, 
                   -prev_point_y - width, 
                   -prev_point_z);
        
        glVertex3f(-point_x + width * cos(theta_right), 
                   -point_y, 
                   -point_z + width * sin(theta_right));
        glVertex3f(-prev_point_x + width * cos(theta_right), 
                   -prev_point_y, 
                   -prev_point_z + width * sin(theta_right));

        glEnd();
        
        
        prev_width = width;
        prev_point_x = point_x;
        prev_point_y = point_y;
        prev_point_z = point_z;
        ++point_itr;
        ++point_index;
    }
    glEnd();
    pthread_mutex_unlock(&points_mutex);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    
    //printf("-------\n");
}