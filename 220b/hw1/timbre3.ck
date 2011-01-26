
TriOsc m => SqrOsc c => HPF hf => LPF lf => Gain g => dac;
2 => c.sync;

20 => c.freq;
60 => m.freq;
30 => m.gain;

100 => hf.freq;
100 => hf.Q;

150 => lf.freq;
100 => lf.Q;

0.05 => lf.gain;

0.1 => g.gain;

while (true)
{
    Std.rand2f(0.1, 3)::second => dur length;
    now + length => time later;
    now => time start;
    
    m.gain() => float m_gain_start;
    m.freq() => float m_freq_start;
    Std.rand2f(10, 200) => float m_gain_target;
    Std.rand2f(60, 100) => float m_freq_target;
    <<< m_gain_target >>>;
    <<< m_freq_target >>>;
    while (now < later)
    {
        (now - start) / length  => float fraction;
        (m_gain_target - m.gain()) * fraction + m_gain_start => m.gain;
        (m_freq_target - m.freq()) * fraction + m_freq_start => m.freq;

        ms => now;
    }
}