SinOsc l => NRev rev => dac;
rev.mix(0.05);

"/usr/ccrma/web/html/courses/220a/ck/hw/" => string dataDir;
200.0 => float update; // update rate in ms

// new class to manage envelopes
class Env
{
  Step s => Envelope e => blackhole; // feed constant into env
  update::ms => e.duration; // set ramp time
  fun void target (float val) { e.target(val); }
}

class Amp extends Env // for gain
{
  fun void run() // sample loop to smoothly update gain
  { 
    while (true)
    {
      l.gain(e.last());
      1::samp => now;
    }
  }
  spork ~ run(); // run run
};

class Freq extends Env // for freq
{
  fun void run()
  { 
    while (true)
    {
      l.freq(e.last());
      1::samp => now;
    }
  }
  spork ~ run(); // run run
};
Amp amp; // instantiate both
Freq freq;

DataReader drywhite;
drywhite.setDataSource(dataDir + "hw4-drywhite.dat");
drywhite.start(); 

while (!drywhite.isFinished())
{
    // next data points, scaled in 0.0 - 1.0 range
    drywhite.scaledVal() => float w; 
    amp.target(0.5 * Math.pow(w, 2.0));
    freq.target(Std.mtof(80.0 + w*20.0));

    update::ms => now;
}
