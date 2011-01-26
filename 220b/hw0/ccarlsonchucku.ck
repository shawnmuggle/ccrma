TriOsc s => JCRev r => Chorus d => r => d  => LPF f => r  => f => d => dac;? 0.5 => r.mix;
while(Std.rand2(4,400)::ms=>now){d.modFreq(Std.rand2f(0.01, 0.9));? d.modDepth(Std.rand2f(0.1,4));
Std.rand2f(20.0,300.0)? => s.freq;Std.rand2f(0.1,0.9) => r.mix; Std.rand2f(20,400) => f.freq; Std.rand2f(0.4,50) => f.Q;}