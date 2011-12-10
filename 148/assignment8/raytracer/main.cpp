// main.cpp
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Scene.h"

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        std::string filename = argv[i];
        clock_t start, end;
        double elapsed;
        start = clock();
        Scene *scene = new Scene(filename);
        end = clock();
        elapsed = ((double) end - start) / CLOCKS_PER_SEC;
        printf("%s took %.2lf seconds.\n", filename.c_str(), elapsed);
        // Welp.
        delete scene;
    }
    return 0;
}
