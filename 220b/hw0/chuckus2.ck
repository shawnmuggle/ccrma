SqrOsc s => TriOsc t => Gain g => LPF f => g => dac;

2 => s.freq;
2 => f.Q;
t.sync(1);

while(true)
{
    ms=>now;
    t.last() * 10 => f.freq;
}