// TURN IT DOWN _SO HARD_ BEFORE YOU PLAY THIS

SinOsc s => DelayA d => d => Gain g => dac; 0.02 => g.gain; second=>d.max;
while (10::ms=>now) { Std.rand2f(0.1, 100)::ms => d.delay;
s.freq() * 0.999 => s.freq; }