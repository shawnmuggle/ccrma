SinOsc l => dac.chan(0);
SinOsc r => dac.chan(1);
l => NRev rev => dac;
r => rev;
rev.mix(0.05);
rev.gain(0.2);


"/Users/mrotondo/Development/ccrma/220a/hw4/" => string dataDir;

DataReader drywhite;
drywhite.setDataSource(dataDir + "hw4-drywhite.dat");
drywhite.start(); 

DataReader fortif;
fortif.setDataSource(dataDir + "hw4-fortif.dat");
fortif.start(); 

while (!drywhite.isFinished() && !fortif.isFinished())
{
    // next data points, scaled in 0.0 - 1.0 range
    drywhite.scaledVal() => float w; 
    fortif.scaledVal() => float p;

    l.gain(0.5 * Math.pow(w, 2.0));
    //l.gain(0.5 * w);
    l.freq(Std.mtof(80.0 + w*20.0));

    r.gain(0.5 * Math.pow(p, 2.0));
    //r.gain(0.5 * p);
    r.freq(Std.mtof(50.0 + p*20.0));
    10::ms => now;
}

