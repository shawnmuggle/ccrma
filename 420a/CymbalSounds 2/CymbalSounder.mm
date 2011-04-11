#import "CymbalSounder.h"

#import "RtAudio.h"
#import "Mesh2D.h"

#import "MikeMesh.h"

#import "FileWvOut.h"

stk::Mesh2D* mesh;
MikeMesh* mikeMesh;

int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *userData )
{
    unsigned int i, j;
    double *buffer = (double *) outputBuffer;
    
    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
    
    float sample = 0;
    
    // Write interleaved audio data.
    for ( i=0; i<nBufferFrames; i++ ) {
        sample = mikeMesh->tick();
        //printf("Sample: %f\n", sample);
        for ( j=0; j<2; j++ ) {
            *buffer++ = sample;
        }
    }
    
    return 0;
}

void* startThread(void* aDac)
{
    RtAudio* dac = (RtAudio*)aDac;
    
    mesh = new stk::Mesh2D(22, 22);
    mesh->setInputPosition(0.33, 0.33);
    mesh->setDecay(1.0);
    mesh->noteOn(10.0, 1.0, 0.5);
    
    mikeMesh = new MikeMesh();
    mikeMesh->noteOn();

    stk::FileWvOut* fileOut = new stk::FileWvOut("out.wav");
    for (int i = 0; i < 44100 * 10; i++) {
        fileOut->tick(mikeMesh->tick());
    }
    
    fileOut->closeFile();
    
    printf("OMG DONE\n");
    
//    RtAudio::StreamParameters parameters;
//    parameters.deviceId = dac->getDefaultOutputDevice();
//    parameters.nChannels = 2;
//    parameters.firstChannel = 0;
//    unsigned int sampleRate = 44100;
//    unsigned int bufferFrames = 256; // 256 sample frames
//    
//    try {
//        dac->openStream( &parameters, NULL, RTAUDIO_FLOAT64,
//                       sampleRate, &bufferFrames, &saw, NULL );
//        dac->startStream();
//    }
//    catch ( RtError& e ) {
//        e.printMessage();
//        exit( 0 );
//    }
    
    return NULL;
}

@implementation CymbalSounder

- (id) init
{
    self = [super init];
    if (self) {
        RtAudio* dac = new RtAudio();
        
        NSLog(@"Current api: %d", dac->getCurrentApi());
        
        if ( dac->getDeviceCount() < 1 ) {
            std::cout << "\nNo audio devices found!\n";
            exit( 0 );
        }

        pthread_create(&rtaudio_thread, NULL, startThread, (void*)dac);
    }
    return self;
}

- (IBAction)setAttackFalloff:(NSSlider*)sender
{
    float falloff = [sender floatValue];
    mikeMesh->setAttackFalloff(falloff);
}


- (IBAction)playCymbal:(id)sender
{
    NSLog(@"YUP");
    //mesh->noteOn(10.0, 1.0, 0.5);
    mikeMesh->noteOn();
}

- (IBAction)tick:(id)sender
{
    mikeMesh->tick();
    mikeMesh->print();
    NSLog(@"----------");
}

@end
