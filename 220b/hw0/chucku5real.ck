fun void bells(int count)
{
    if (count == 0)
        return;
    BandedWG m => NRev n => Envelope e => dac;
    Std.rand2(0,3) => m.preset;
    Std.rand2(20, 600) => m.freq;
    1 => e.value => m.pluck;
    1::second=>now;
    spork ~ bells(count - 1);
    10::second => e.duration;
    0 => e.target;
    while(true){second=>now;}
}
10 => int count;
spork ~ bells(count);
now + count::second * 2 => time ring;
while (ring > now)
{
    second => now;
}