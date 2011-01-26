Noise m => SinOsc c => Gain g => dac;
2 => c.sync;

2000 => c.freq;
6000 => m.gain;

0.1 => g.gain;

while (true)
{
    c.freq() * 0.99999 => c.freq;
    m.gain() * 0.99999 => m.gain;
    samp => now;
}