//
//  MikeMesh.cpp
//  CymbalSounds
//
//  Created by Michael Rotondo on 3/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "MikeMesh.h"

MikeMesh::MikeMesh()
{
    input = 0.0;
    
    falloff = 0.9;
    
    inputX = 0;
    inputY = 0;
    
    numTicks = 0;
    
    meshWidth = 30;
    meshHeight = 30;
    junctions.resize(meshWidth * meshHeight);
    
    Junction* j;
    APNLEdge* e;

    // Row by row...
    for (int y = 0; y < meshHeight; y++) {
        for (int x = 0; x < meshWidth; x++) {
            j = new Junction();
            
            junctions[y * meshWidth + x] = j;
            
            if (x == 0) {
                e = new APNLEdge();
                e->left = true;
                reflections.push_back(e);
                
                j->left = new MikeString();
                e->setString(j->left);
                
                
                j->left->x = x;
                j->left->y = y;
                j->left->label = "left";
                strings.push_back(j->left);
                

                j->left->setAInput(e);
                j->left->setBInput(j);
            }
            if (y == 0) {
                e = new APNLEdge();
                e->top = true;
                reflections.push_back(e);
                
                j->top = new MikeString();
                e->setString(j->top);

                
                j->top->x = x;
                j->top->y = y;
                j->top->label = "top";
                strings.push_back(j->top);
                
                
                j->top->setAInput(e);
                j->top->setBInput(j);
            }
            if (x == meshWidth - 1) {
                e = new APNLEdge();
                e->right = true;
                reflections.push_back(e);
                
                j->right = new MikeString();
                e->setString(j->right);

                
                j->right->x = x;
                j->right->y = y;
                j->right->label = "right";
                strings.push_back(j->right);
                
                
                j->right->setAInput(j);
                j->right->setBInput(e);
            }
            if (y == meshHeight - 1) {
                e = new APNLEdge();
                e->bottom = true;
                reflections.push_back(e);
                
                j->bottom = new MikeString();
                e->setString(j->bottom);
                
                
                j->bottom->x = x;
                j->bottom->y = y;
                j->bottom->label = "bottom";
                strings.push_back(j->bottom);
                

                j->bottom->setAInput(j);
                j->bottom->setBInput(e);
            }
            
            if (x > 0) {
                Junction* leftJunction = junctions[y * meshWidth + x - 1];
                j->left = leftJunction->right;
                j->left->setBInput(j);
            }
            if (x < meshWidth - 1) {
                MikeString* rightString = new MikeString();
                j->right = rightString;
                rightString->setAInput(j);
                
                rightString->x = x;
                rightString->y = y;
                rightString->label = "right";
                strings.push_back(rightString);
                
            }

            if (y > 0) {
                Junction* topJunction = junctions[(y - 1) * meshWidth + x];
                j->top = topJunction->bottom;
                j->top->setBInput(j);
            }
            if (y < meshHeight - 1) {                
                MikeString* bottomString = new MikeString();
                j->bottom = bottomString;
                bottomString->setAInput(j);
                
                bottomString->x = x;
                bottomString->y = y;
                bottomString->label = "bottom";
                strings.push_back(bottomString);
            }
        }
    }
    
    noteOn();
    falloff = 0.0;
}

stk::StkFloat MikeMesh::tick()
{
    numTicks++;

    // STEP 1: Figure out the next output value from each junction and reflection
    std::vector<Connection*>::iterator it = reflections.begin();
    while(it != reflections.end()) {
        Connection* c = *it;
        c->tick(numTicks);
        it++;
    }
    std::vector<Junction*>::iterator it2 = junctions.begin();
    while(it2 != junctions.end()) {
        Connection* c = *it2;
        c->tick(numTicks);
        it2++;
    }
    
    // STEP 2: For each string, tick it with the outgoing value from the connection it's flowing out from
    std::vector<MikeString*>::iterator it3 = strings.begin();
    while(it3 != strings.end()) {
        MikeString* string = *it3;
        string->tick(numTicks);
        it3++;
    }
    
    stk::StkFloat output = junctions[0]->getValue();
    return output;
}

void MikeMesh::print()
{
    for (int y = 0; y < meshHeight; y++)
    {
        for (int x = 0; x < meshWidth; x++)
        {
            Junction* j = junctions[y * meshWidth + x];
            printf("%f\t", j->getValue());
        }
        printf("\n");
    }
    
    std::vector<MikeString*>::iterator it = strings.begin();
    while(it != strings.end()) {
        MikeString* string = *it;
        printf("String: %d, %d: ", string->x, string->y);
        printf(string->label.c_str());
        printf(" - a: %f, b: %f\n", string->a.nextOut(), string->b.nextOut());
        it++;
    }
}

void MikeMesh::noteOn()
{
    junctions[0]->left->a.tick(9.5);
    junctions[0]->left->b.tick(9.5);
}

void MikeMesh::setAttackFalloff(float newFalloff)
{
    falloff = newFalloff;
}

Junction::Junction()
{
    ticks = 0;
    output = 0.0;
}

void Junction::tick(int numTicks)
{
    output = (left->getAValue() + top->getAValue() + right->getBValue() + bottom->getBValue()) * 0.5;
}

float Junction::getValue()
{
    return output;
}

void Junction::setValue(float newValue)
{
    output = newValue;
}

MikeString::MikeString()
{
    // TODO: Parameterize this!
    float delay = 18 + rand() % 25;
    a.setDelay(delay);
    b.setDelay(delay);
    
    aTicks = 0;
    bTicks = 0;
}

void MikeString::tick(int numTicks)
{
    float aInput = aIn->output;
    
    Junction* j = dynamic_cast<Junction*>(aIn);
    if (j != NULL) {
        aInput -= b.nextOut(); // MIGHT HAVE TO SWITCH THIS SIGN
    } else {
        //printf("A got a value from a reflection %d : %f\n", aIn, aInput);
    }

    float bInput = bIn->output;
    
    Junction* j2 = dynamic_cast<Junction*>(bIn);
    if (j2 != NULL) {
        bInput -= a.nextOut(); // MIGHT HAVE TO SWITCH THIS SIGN
    } else {
        //printf("B got a value from a reflection %d : %f\n", bIn, bInput);
    }
    
    a.tick(aInput);
    b.tick(bInput);
}

stk::StkFloat MikeString::getAValue()
{
    //return a.lastOut();
    return a.nextOut();
}

stk::StkFloat MikeString::getBValue()
{
    //return b.lastOut();
    return b.nextOut();
}

void MikeString::setAInput(Connection* connection)
{
    aIn = connection;
}
void MikeString::setBInput(Connection* connection)
{
    bIn = connection;
}

void ClampedEdge::tick(int numTicks)
{    
    float in;
    if (left || top) {
        in = string->getBValue();
    } else {
        in = string->getAValue();
    }
    
    //printf("HEY REFLECTING A VALUE %f %d \n", in, this);
    
    output = transform(in);
}

float ClampedEdge::getValue()
{
    return output;
}

void ClampedEdge::setValue(float newValue)
{
    //output = newValue;
}

void ClampedEdge::setString(MikeString* newString)
{
    string = newString;
}

float ClampedEdge::transform(float input)
{
    return -0.999 * input;
}

APNLEdge::APNLEdge()
{
    filter = new stk::NonLinearAllPass();
}

void APNLEdge::tick(int numTicks)
{    
    float in;
    if (left || top) {
        in = string->getBValue();
    } else {
        in = string->getAValue();
    }
    
    //printf("HEY REFLECTING A VALUE %f %d \n", in, this);
    
    output = transform(in);
}

float APNLEdge::getValue()
{
    return output;
}

void APNLEdge::setValue(float newValue)
{
    //output = newValue;
}

void APNLEdge::setString(MikeString* newString)
{
    string = newString;
}

float APNLEdge::transform(float input)
{
    return -filter->tick(input);
}
