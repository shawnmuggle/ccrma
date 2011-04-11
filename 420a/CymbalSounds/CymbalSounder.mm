#import "CymbalSounder.h"

#import "RtAudio.h"
#import "Mesh2D.h"

#import "MikeMesh.h"

stk::Mesh2D* mesh;
MikeMesh* mikeMesh;

// Two-channel sawtooth wave generator.
int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *userData )
{
    unsigned int i, j;
    double *buffer = (double *) outputBuffer;
    
    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
    
    // Write interleaved audio data.
    for ( i=0; i<nBufferFrames; i++ ) {
        for ( j=0; j<2; j++ ) {
            //*buffer++ = mesh->tick();
            *buffer++ = mikeMesh->tick();
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
    //mikeMesh->noteOn();
    
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac->getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256; // 256 sample frames
    
    try {
        dac->openStream( &parameters, NULL, RTAUDIO_FLOAT64,
                       sampleRate, &bufferFrames, &saw, NULL );
        dac->startStream();
    }
    catch ( RtError& e ) {
        e.printMessage();
        exit( 0 );
    }
    
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

- (IBAction)setSize:(NSSlider*)sender
{
    float length = [sender floatValue];
    mikeMesh->setSize(length);
}
- (IBAction)setAttackFalloff:(NSSlider*)sender
{
    float falloff = [sender floatValue];
    mikeMesh->setAttackFalloff(falloff);
}

- (IBAction)setLeftPole:(NSSlider*)sender
{
    float pole = [sender floatValue];
    mikeMesh->setLeftPole(pole);
}
- (IBAction)setLeftDamping:(NSSlider*)sender
{
    float damping = [sender floatValue];
    mikeMesh->setLeftDamping(damping);
}
- (IBAction)setLeftA1:(NSSlider*)sender
{
    float coefficient = [sender floatValue];
    mikeMesh->setLeftA1(coefficient);
}
- (IBAction)setLeftA2:(NSSlider*)sender
{
    float coefficient = [sender floatValue];
    mikeMesh->setLeftA2(coefficient);
}

- (IBAction)setRightPole:(NSSlider*)sender
{
    float pole = [sender floatValue];
    mikeMesh->setRightPole(pole);
}
- (IBAction)setRightDamping:(NSSlider*)sender
{
    float damping = [sender floatValue];
    mikeMesh->setRightDamping(damping);
}
- (IBAction)setRightA1:(NSSlider*)sender
{
    float coefficient = [sender floatValue];
    mikeMesh->setRightA1(coefficient);
}
- (IBAction)setRightA2:(NSSlider*)sender
{
    float coefficient = [sender floatValue];
    mikeMesh->setRightA2(coefficient);
}


- (IBAction)playCymbal:(id)sender {
    NSLog(@"YUP");
    //mesh->noteOn(10.0, 1.0, 0.5);
    mikeMesh->noteOn();
}
@end
