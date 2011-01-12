SqrOsc s=>LPF l=>Envelope e=>e=>Echo c=>ResonZ r=>dac; 600=>r.freq; 0.2=>c.mix;
15::ms=>c.delay; while(150::ms=>now) {Std.rand2(10,100)=>s.freq=>l.freq;
l.freq()*5::ms=>e.duration; 1=>e.value; e.keyOff();}