SqrOsc m => SqrOsc c => LPF f => ADSR e => dac;
2 => c.sync;

0.1 => e.gain;

Std.rand2f(30, 60) => c.freq;
Std.rand2f(10, 50) => m.freq;
Std.rand2f(200, 600) => m.gain;

800 => float f_freq;

800 => f.freq;
4 => f.Q;


Std.rand2f(250, 850)::ms => dur length;
10::ms => e.attackTime;
length - 20::ms => e.decayTime;
0.2 => e.sustainLevel;
Std.rand2f(40, 50)::ms => e.releaseTime;

e.keyOn();
now + e.attackTime() + e.decayTime() => time later;
while (now < later)
{
    m.gain() * 0.999 => m.gain;
    ms => now;
}
length => now;
e.keyOff();
e.releaseTime() => now;