// evolve pan, evolve gains

15 => int n;
1 => int num_playing;

NRev r => dac;
0.01 => r.gain;
0.5 => r.mix;

SqrOsc modulators[n];
TriOsc carriers[n];
HPF hipass_filters[n];
Pan2 pans[n];

// Figure out a way to do this with Envelope?
float carrier_freq_slews[n];
float modulator_freq_slews[n];
float modulation_index_slews[n];
float hpf_freq_slews[n];
float hpf_q_slews[n];
float pan_slews[n];

for (0 => int i; i < n; i++)
{
    // Maybe add a compressor here to squash crazy feedback squeaks
    modulators[i] => carriers[i] => hipass_filters[i] => pans[i] => r;
    2 => carriers[i].sync;
}

fun void create_buzz()
{
    for (0 => int i; i < n; i++)
    {        
        Std.rand2f(800, 1500) => carriers[i].freq;
        Std.rand2f(80, 500) => modulators[i].freq;
        Std.rand2f(100, 800) => modulators[i].gain;
        
        Std.rand2f(500, 3000) => hipass_filters[i].freq;
        Std.rand2f(1.1, 2) => hipass_filters[i].Q; 
               
        Std.rand2f(0, 0.2) => hipass_filters[i].gain;
        
        Std.rand2f(-1, 1) => pans[i].pan;
        0 => pans[i].gain;
    }
}

fun void create_slews()
{
    for (0 => int i; i < n; i++)
    {
        Std.rand2f(-0.05, 0.05) => carrier_freq_slews[i];
        Std.rand2f(-0.05, 0.05) => modulator_freq_slews[i];
        Std.rand2f(-0.05, 0.05) => modulation_index_slews[i];
        Std.rand2f(-0.05, 0.05) => hpf_freq_slews[i];
        Std.rand2f(-0.05, 0.05) => hpf_q_slews[i];
        Std.rand2f(-0.001, 0.001) => pan_slews[i];
    }
}

fun void buzz()
{    
    create_buzz();
    while (true)
    {
        create_slews();
        Std.rand2f(1, 3)::second => dur length;
        
        now + length => time stop;
        while (now < stop)
        {
            for (0 => int i; i < n; i++)
            {        
                Math.max(0.1, carriers[i].freq() + carrier_freq_slews[i]) => carriers[i].freq;
                Math.max(0.1, modulators[i].freq() + modulator_freq_slews[i]) => modulators[i].freq;
                Math.max(0.1, modulators[i].gain() + modulation_index_slews[i]) => modulators[i].gain;
                Math.min(2000, Math.max(0.1, hipass_filters[i].freq() + hpf_freq_slews[i])) => hipass_filters[i].freq;
                Math.max(1.1, hipass_filters[i].Q() + hpf_q_slews[i]) => hipass_filters[i].Q;
                
                Math.min(1, Math.max(-1, pans[i].pan() + pan_slews[i])) => pans[i].pan;
                
                if (i < num_playing)
                {
                    Math.min(1, pans[i].gain() + 0.01) => pans[i].gain;
                }
                else 
                {
                    Math.max(0, pans[i].gain() - 0.01) => pans[i].gain;
                }
            }
            ms => now;
        }
    }
}

spork ~ buzz();

while (num_playing < 15)
{
    Std.rand2f(1, 4)::second => now;
    num_playing++;
}
while (num_playing > 0)
{
    num_playing--;
    Std.rand2f(1, 4)::second => now;
}    
