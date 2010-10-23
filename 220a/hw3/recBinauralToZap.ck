// chuck this with other shreds to record to file

// arguments: rec:<seconds>
me.arg(0) => string tmp; // get user spec'd duration
float s;
if( tmp.length() == 0 ) // or not?
{
    35.0 => s;
    <<<"no duration for recording specified, defaulting to 10 seconds">>>;
    } else {
    Std.atof(tmp) => s;
    }
string filename0; // files will be written into current directory
"/zap/chuckout-0.wav" => filename0;
string filename1;
"/zap/chuckout-1.wav" => filename1;
<<< "recording mono sound files ",filename0," ",filename1," for ",s," seconds">>>;

// pull samples from the dac
dac.chan(0) => Gain g => WvOut w0 => blackhole;
dac.chan(1) => g => WvOut w1 => blackhole;
0.7 => g.gain;

// this is the output file name
filename0 => w0.wavFilename;
filename1 => w1.wavFilename;

now + s::second => time then;
while( now < then )
{
100::ms => now;
}

w0.closeFile();
w1.closeFile();

<<<"done recording, to mix into stereo run the following command in a terminal:">>>;
<<<"sox -M  /zap/chuckout-0.wav /zap/chuckout-1.wav /zap/merged-stereo.wav">>>;
