SqrOsc s => LPF lf => Envelope e => e => dac;
Noise n => BPF lf2 => Envelope e2;// => dac;
2 => lf.Q;
8 => lf2.Q;
18 => n.gain;
while(true)
{
    1.0 => e.value => e2.value;
    0.0 => e.target => e2.target;
    Std.rand2f(10,500)::ms => dur t => e.duration;
    t * 0.1 => e2.duration;
    Std.rand2f(20, 100) => float f => s.freq;
    f * 0.8 => lf.freq;
    f * 8 => lf2.freq;
    150::ms=>now;
}