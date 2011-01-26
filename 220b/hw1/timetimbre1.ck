// sweep, parameterize depth, vlfo rate, lfo rate

SinOsc m => SinOsc c => dac;
2 => c.sync;

50 => c.freq;
49 => m.freq;
200 => m.gain;

SinOsc vlfo => SinOsc lfo => blackhole;
2 => lfo.sync;

0.2 => lfo.freq;
1.2 => vlfo.freq;
1 => vlfo.gain;


0.9 => c.gain;

while (true)
{
    
    m.gain() + lfo.last() => m.gain;
    1::ms => now;
}