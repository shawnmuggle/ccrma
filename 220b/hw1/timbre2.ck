// generate random number in uniform distribution [0,1]
fun float uniform()
{ return Std.rand2f(0,1); }

// generate a random number in exponentail distribution
fun float expo( float alpha )
{ return -Math.log(1-uniform()) / alpha; }

fun void chick()
{
    SinOsc m => SqrOsc c => BPF f => ADSR e => Gain g => Pan2 p => dac;
    2 => c.sync;
    
    0.1 => g.gain;
    
    Std.rand2f(500, 1000) => c.freq;
    Std.rand2f(5, 30) => m.freq;
    Std.rand2f(200, 1800) => m.gain;
    
    Std.rand2f(250, 2550) => f.freq;
    Std.rand2f(20, 50) => f.Q;
    
    Std.rand2f(5, 10)::ms => e.attackTime;
    2::ms => e.releaseTime;
    
    Std.rand2f(-0.5, 0.5) => p.pan;
    
    5 => f.gain;
    
    e.keyOn();
    e.attackTime() => now;
    e.keyOff();
    e.releaseTime() => now;
}

1 => float num_per_sec;
1 => float offset;

fun void play_chicks()
{
    while (true)
    {
        spork ~ chick();
        <<< num_per_sec >>>;
        expo(num_per_sec)::second => now;
    }
}

spork ~ play_chicks();


TriOsc lfo => blackhole;
(1.0/32.0) => lfo.freq;

now + 16::second => time later;
while (now < later)
{
    Math.fabs(lfo.last()) * 10 + offset => num_per_sec;
    ms => now;
}