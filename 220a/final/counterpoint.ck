fun float SumFloats(float floats[])
{
    0 => float sum;
    for (0 => int i; i < floats.cap(); i++)
    {
        sum + floats[i] => sum;
    }
    return sum;
}

class CantusFirmus
{
    0 => int done;
    0 => int resolve;
    60 => int root;
    root => int prev_pitch;
    [0, 2, 4, 5, 7, 8] @=> int intervals[];
    0 => int notes_played;
    
    // start on the root
    [1.0, 0.0, 0.0, 0.0, 0.0, 0.0] @=> float weights[];
    
    fun int Done()
    {
        return done;
    }
    
    fun int GetNextPitch()
    {
        <<< "Getting next pitch!" >>>;
        
        SumFloats(weights) => float weights_sum;
        Std.rand2f(0, weights_sum) => float rand_val;
        
        int next_interval_index;
        0 => float accum;
        for (0 => int i; i < weights.cap(); i++)
        {
            accum + weights[i] => accum;
            0 => weights[i]; // clear weights, will be reset afterwards
            if (accum > rand_val)
            {
                i => next_interval_index;
                break;
            }
        }
        
        <<< "Next interval index: ", next_interval_index >>>;
        
        root + intervals[next_interval_index] => int next_pitch;
        
        if (next_pitch - prev_pitch > 4)
        {
            1.0 => weights[next_interval_index - 1];
        }
        else if (next_pitch - prev_pitch < -4)
        {
            1.0 => weights[next_interval_index + 1];
        }
        else
        {
            // Assign new weights
            for (0 => int i; i < weights.cap(); i++)
            {
                
                Std.abs(i - next_interval_index) => int dist;
                Math.pow(2, -dist) => weights[i];
                
                // never choose the same pitch
                if (i == next_interval_index)
                {
                    0 => weights[i];
                }
            }
        }
        
        if (resolve == 1)
        {
            root => next_pitch;
            1 => done;
        }
        
        
        if (Std.abs(next_pitch % root) == 2)
        {
            if (notes_played > 5 && Std.rand2f(0, 1) > 0.5)
            {
                1 => resolve;
            }
            
        }
        
        1 +=> notes_played;
        next_pitch => prev_pitch;
        return next_pitch;
    }
}

class ParallelFifthsCounterpoint
{
    CantusFirmus @cf;
    
    fun int GetNextPitch()
    {
        return cf.prev_pitch + 7;
    }
    
}

CantusFirmus cf;
ParallelFifthsCounterpoint fifths;
cf @=> fifths.cf;

SinOsc cf_sin => dac;
1.0 => cf_sin.gain;

SinOsc cp_sin => dac;
1.0 => cp_sin.gain;

while (true)
{
    Std.mtof(cf.GetNextPitch()) => cf_sin.freq;
    Std.mtof(fifths.GetNextPitch()) => cp_sin.freq;
    500::ms => now;
    if (cf.Done())
    {
        break;
    }
}