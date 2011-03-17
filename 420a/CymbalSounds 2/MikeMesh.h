//
//  MikeMesh.h
//  CymbalSounds
//
//  Created by Michael Rotondo on 3/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MIKEMESH_H
#define MIKEMESH_H

#include "DelayA.h"
#include "NonLinearAllPass.h"

#include <vector>
#include <string>

class Connection
{
public:
    virtual void tick(int numTicks) = 0;
    virtual float getValue() = 0;
    virtual void setValue(float newValue) = 0;
    
    float output;
    
protected:
    int ticks;
};

class Junction;
class MikeString;

class MikeMesh
{
public:
    MikeMesh();
    ~MikeMesh();
  
    void noteOn();
    stk::StkFloat tick();
    
    void setAttackFalloff(float falloff);    
    
    void print();

protected:
    stk::StkFloat input;
    stk::StkFloat falloff;
    
    std::vector<Junction*> junctions;
    std::vector<Connection*> reflections;
    std::vector<MikeString*> strings;
    int meshWidth, meshHeight;
    
    int numTicks;
    
    int inputX, inputY;
};

class MikeString
{
public:
    MikeString();
//    ~MikeString();
    
    stk::StkFloat getAValue();
    stk::StkFloat getBValue();
    
    void tick(int numTicks);
    
    void setAInput(Connection* connection);
    void setBInput(Connection* connection);

    void setLength(float lengthSamps);
    
    // DEBUG INFO
    std::string label;
    int x, y;

    // Should be protected
    stk::DelayA a, b;
protected:

    int aTicks, bTicks;
    Connection* aIn;
    Connection* bIn;
};

class ClampedEdge : public Connection
{
public:
//    ClampedEdge();
//    ~ClampedEdge();
    // TODO: Consider making edges have strings instead of bools, like Junctions.
    bool left, top, right, bottom;

    void setString(MikeString* newString);
    
    virtual void tick(int numTicks);
    virtual float getValue();
    virtual void setValue(float newValue);
protected:
    MikeString* string;
    float transform(float input);
};

class Junction : public Connection
{
public:
    Junction();
//    ~Junction();
    MikeString* left;
    MikeString* right;
    MikeString* top;
    MikeString* bottom;

    virtual void tick(int numTicks);
    virtual float getValue();
    virtual void setValue(float newValue);
protected:
    
};

class APNLEdge : public Connection
{
public:
    APNLEdge();
    
    bool left, top, right, bottom;
    void setString(MikeString* newString);
    
    virtual void tick(int numTicks);
    virtual float getValue();
    virtual void setValue(float newValue);
protected:
    MikeString* string;
    stk::NonLinearAllPass* filter;
    float transform(float input);
};

#endif