// we want sparkly highpitchness

SqrOsc m1 => TriOsc c1 => HPF hf1 => dac;
2 => c1.sync;

600 => c1.freq;
100 => m1.freq;
500 => m1.gain;

2000 => hf1.freq;
2 => hf1.Q;

SqrOsc m2 => TriOsc c2 => HPF hf2 => dac;
2 => c2.sync;

800 => c2.freq;
200 => m2.freq;
423 => m2.gain;

2200 => hf2.freq;
2 => hf2.Q;

SqrOsc m3 => TriOsc c3 => HPF hf3 => dac;
2 => c3.sync;

1100 => c3.freq;
300 => m3.freq;
310 => m3.gain;

2300 => hf3.freq;
2 => hf3.Q;

while (true)
{
    second => now;
}