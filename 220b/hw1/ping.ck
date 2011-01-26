// ping

SinOsc m => SinOsc c => HPF hf => LPF lf => ADSR e => NRev r => Gain g => dac;
2 => c.sync;


0.5 => g.gain;

2000 => c.freq;
1000 => m.freq;
2400 => m.gain;

1350 => lf.freq;
2 => lf.Q;

450 => hf.freq;
100 => hf.Q;

10::ms => e.attackTime;
40::ms => e.decayTime;
0.1 => e.sustainLevel;
100::ms => e.releaseTime;
e.keyOn();
80::ms => now;
e.keyOff();
5::second => now;