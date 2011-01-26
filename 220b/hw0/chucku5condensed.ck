fun void b(int c, dur d){if (c != 0){BandedWG m => NRev n => Envelope e => dac;Std.rand2(0,3) => m.preset;
Std.rand2(20, 600) => m.freq;1 => e.value => m.pluck;1::second=>now;spork ~ b(c - 1, d);d => e.duration;
0 => e.target;while(true){second=>now;}}}10 => int c;spork ~ b(c, c::second);now + c::second * 2 => time r;
while (r > now){second => now;}