// drumz, parameterize pitch, falloff, etc

fun void drum()
{
    TriOsc m => SqrOsc c => BPF bf1 => Gain g => Pan2 p => dac;
    c => BPF bf2 => dac;
    2 => c.sync;
    
    Std.rand2f(300, 700) => c.freq;
    Std.rand2f(20, 60) => m.freq;
    200 => m.gain;
    
    100 => bf1.freq;
    4 => bf1.Q;
    
    430 => bf2.freq;
    200 => bf2.Q;
    
    3 => int bf2FreqTarget;
    500 => int bf1QTarget;
    
    Std.rand2f(-1, 1) => p.pan;
    
    Std.rand2f(0.005, 0.01) => float slew;
    
    now + 800::ms => time later;
    
    while (now < later)
    {
        (bf2FreqTarget - bf2.freq()) * slew + bf2.freq() => bf2.freq;
        (bf1QTarget - bf1.Q()) * slew + bf1.Q() => bf1.Q;
        g.gain() * 0.99 => g.gain;
        1::ms => now;
    }
}

while (true)
{
    Std.rand2f(0, 1)::second => now;
    spork ~ drum();
}