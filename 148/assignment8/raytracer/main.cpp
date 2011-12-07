// main.cpp
#include <stdio.h>
#include <string.h>
#include "Scene.h"

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        std::string filename = argv[i];
        Scene *scene = new Scene(filename);
        // Welp.
        delete scene;
    }
    return 0;
}
