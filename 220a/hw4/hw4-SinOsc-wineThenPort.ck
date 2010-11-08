SinOsc s => dac;
"/Users/mrotondo/Development/ccrma/220a/hw4/" => string dataDir;
// instantiate reader
DataReader drywhite;
// set data file
drywhite.setDataSource(dataDir + "hw4-drywhite.dat");
// calibrate and start from beginning
drywhite.start(); 

while (!drywhite.isFinished())
{
    drywhite.scaledVal() => float val; // next data point, scaled in 0.0 - 1.0 range

    s.gain(0.5 * val);
    s.freq(Std.mtof(80.0 + val*20.0));
    40::ms => now;
}

// mute for 1 second
s.gain(0.0);
1::second => now;

DataReader fortif;
fortif.setDataSource(dataDir + "hw4-fortif.dat");
fortif.start(); 

while (!fortif.isFinished())
{
    fortif.scaledVal() => float val; // next data point, scaled in 0.0 - 1.0 range

    s.gain(0.5 * val);
    s.freq(Std.mtof(80.0 + val*20.0));
    40::ms => now;
}