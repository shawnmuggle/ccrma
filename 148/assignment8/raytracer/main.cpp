// main.cpp
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Scene.h"
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        std::string filename = argv[i];
        uint64_t        start;
        uint64_t        end;
        uint64_t        elapsed;
        uint64_t        elapsedNano;
        static mach_timebase_info_data_t    sTimebaseInfo;
        start = mach_absolute_time();
        Scene *scene = new Scene(filename);
        end = mach_absolute_time();
        elapsed = end - start;
        
        if ( sTimebaseInfo.denom == 0 ) {
            (void) mach_timebase_info(&sTimebaseInfo);
        }
        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        
        printf("%s took %.2f seconds.\n", filename.c_str(), elapsedNano / 1000000000.0);

        // Welp.
        delete scene;
    }
    return 0;
}
