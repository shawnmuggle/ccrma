// main.cpp
#include <stdio.h>
#include <string.h>
#include "Scene.h"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: raytracer <scene_filename>\n");
    }
    
    std::string filename = argv[1];
    
    Scene *scene = new Scene(filename);
    // Welp.
    delete scene;
    
    return 0;
}
