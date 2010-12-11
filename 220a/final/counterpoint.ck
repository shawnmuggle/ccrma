fun float SumFloats(float floats[])
{
    0 => float sum;
    for (0 => int i; i < floats.cap(); i++)
    {
        sum + floats[i] => sum;
    }
    return sum;
}

fun void OrderVoices(Voice voices[])
{
    0 => int num_voices_ordered;
    while (num_voices_ordered < voices.cap())
    {
        -1 => int max;
        -1 => int max_index;
        for (0 => int i; i < voices.cap(); i++)
        {
            if (voices[i].order == -1)
            {
                Std.rand() => int rand;
                
                <<< rand >>>;
                if (rand > max)
                {
                    rand => max;
                    i => max_index;
                }
            }
        }
        num_voices_ordered => voices[max_index].order;
        1 +=> num_voices_ordered;
    }
}

class Voice
{
    Voice @ other_voices[1000];
    0 => int num_other_voices;
    
    -1 => int order;
    
    0 => int done;
    0 => int resolve;

    72 => int root;
    root => int pitch;
    0 => int prev_pitch;
    
    0::ms => dur note_duration;
    
    [0, 2, 4, 5, 7, 10, 12] @=> int intervals[];
    
    // start on the root
    [1.0, 0.0, 0.0, 0.0, 0.0, 0.0] @=> float weights[];
    
    // HACK: the subdivisions also act as probability weights for how they are selected.
    [0.5, 1.0] @=> float duration_subdivisions[];
    
    SinOsc osc => dac;
    0.0 => osc.gain;
    
    0 => int playing_subject;
    int subject_pitches[100];
    dur subject_durations[100];
    0 => int subject_length;
    0 => int subject_done;
    
    0 => int num_times_played;
    
    fun void AddOtherVoice(Voice v)
    {
        v @=> other_voices[num_other_voices];
        1 +=> num_other_voices;
    }
    
    fun Voice FirstVoice()
    {
        if (order == 0)
        {
            return this;
        }
        
        for (0 => int i; i < num_other_voices; i++)
        {
            if (other_voices[i].order == 0)
            {
                return other_voices[i];
            }
        }
    }
    
    fun void WaitForPrecedingVoicesToHavePlayedNumTimes(int num_times)
    {
        SetGain(0.0);
        while (true)
        {
            1 => int done_waiting;
            for (0 => int i; i < num_other_voices; i++)
            {
                other_voices[i] @=> Voice @ v;
                if (v.order < order && num_times > v.num_times_played)
                {
                    0 => done_waiting;
                }
            }
            if (1 == done_waiting)
            {
                break;
            }
            1::ms => now;
        }        
    }
    
    fun void Play()
    {
        WaitForPrecedingVoicesToHavePlayedNumTimes(1);
        <<< "ANOTHER VOICE STARTING!" >>>;
        
        if (order == 0)
        {
            <<< "INVENTING A SUBJECT" >>>;
            1 => playing_subject;
            while (subject_done == 0)
            {
                PlayNextNote();
                pitch => subject_pitches[subject_length];
                note_duration => subject_durations[subject_length];
                1 +=> subject_length;
            }
            1 +=> num_times_played;
            PlaySequenceTransformDurations(subject_pitches, subject_durations, subject_length, 2);
            PlayReverseSequenceTransformDurations(subject_pitches, subject_durations, subject_length, 1);
            PlaySequence(subject_pitches, subject_durations, subject_length);
            PlaySequence(subject_pitches, subject_durations, subject_length);
            SetGain(0.0);
        }
        else if (order == 1)
        {
            PlaySequence(FirstVoice().subject_pitches, FirstVoice().subject_durations, FirstVoice().subject_length);
            WaitForPrecedingVoicesToHavePlayedNumTimes(2); // Come in as voice 0 finishes slow version, starts backwards version
            PlaySequenceTransformDurations(FirstVoice().subject_pitches, FirstVoice().subject_durations, FirstVoice().subject_length, 0.5);
            PlayReverseSequenceTransformDurations(FirstVoice().subject_pitches, FirstVoice().subject_durations, FirstVoice().subject_length, 0.5);
            WaitForPrecedingVoicesToHavePlayedNumTimes(4); // come in for unison last verse
            PlaySequence(FirstVoice().subject_pitches, FirstVoice().subject_durations, FirstVoice().subject_length);
            SetGain(0.0);
        }
        else if (order == 2)
        {
            PlaySequence(FirstVoice().subject_pitches, FirstVoice().subject_durations, FirstVoice().subject_length);
            WaitForPrecedingVoicesToHavePlayedNumTimes(3); // Come in as voice 0 finishes slow version, starts backwards version
            PlayReverseSequenceTransformDurations(FirstVoice().subject_pitches, FirstVoice().subject_durations, FirstVoice().subject_length, 1);
            PlaySequence(FirstVoice().subject_pitches, FirstVoice().subject_durations, FirstVoice().subject_length);
            SetGain(0.0);
        }
    }
    
    fun void SetFrequency(float freq)
    {
        SetGain(1.0);
        freq => osc.freq;
    }
    
    fun void SetGain(float gain)
    {
        gain => osc.gain;
    }
    
    fun void PlayReverseSequenceTransformDurations(int pitches[], dur durations[], int length, float duration_factor)
    {
        <<< "Reverse sequence, length is ", length >>>;
        
        // TODO: this would be cleaner if i transformed the durations and then passed it all to PlaySequence
        for (length => int i; i >= 0;  i--)
        {
            <<< "Playing back sequence pitch ", pitches[i] >>>;
            SetFrequency(Std.mtof(pitches[i]));
            durations[i] * duration_factor => now;
        }
        
        1 +=> num_times_played;
    }
    
    fun void PlaySequenceTransformDurations(int pitches[], dur durations[], int length, float duration_factor)
    {
        // TODO: this would be cleaner if i transformed the durations and then passed it all to PlaySequence
        for (0 => int i; i <= length;  i++)
        {
            <<< "Playing back sequence pitch ", pitches[i] >>>;
            SetFrequency(Std.mtof(pitches[i]));
            durations[i] * duration_factor => now;
        }
        
        1 +=> num_times_played;
    }
    
    fun void PlaySequence(int pitches[], dur durations[], int length)
    {
        for (0 => int i; i <= length;  i++)
        {
            <<< "Playing back sequence pitch ", pitches[i] >>>;
            SetFrequency(Std.mtof(pitches[i]));
            durations[i] => now;
        }
        
        1 +=> num_times_played;
    }
    
    fun int Done()
    {
        return done;
    }
    
    fun void PlayNextNote()
    {
        ComputeNextPitch();
        ComputeNextDuration();
        
        <<< "1" >>>;
        
        SetFrequency(Std.mtof(pitch));
        note_duration => now;
        <<< "2" >>>;
    }
    
    fun void ComputeNextDuration()
    {
        SumFloats(duration_subdivisions) => float weights_sum;
        Std.rand2f(0, weights_sum) => float rand_val;
        
        int duration_subdivisions_index;
        0 => float accum;
        for (0 => int i; i < duration_subdivisions.cap(); i++)
        {
            accum + duration_subdivisions[i] => accum;
            if (accum > rand_val)
            {
                i => duration_subdivisions_index;
                break;
            }
        }
        duration_subdivisions[duration_subdivisions_index] * 500::ms => note_duration;
    }
    
    fun void ComputeNextPitch()
    {
        pitch => prev_pitch;

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
        
        root + intervals[next_interval_index] => int next_pitch;
        
        if (next_pitch - pitch > 4)
        {
            1.0 => weights[next_interval_index - 1];
        }
        else if (next_pitch - pitch < -4)
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
            1 => subject_done; // TODO: this should be set up so that the other voices detect the subject being done _AFTER_ the last note plays, not before.
        }
        
        
        if (Std.abs(next_pitch % root) == 2)
        {
            if (subject_length > 5 && Std.rand2f(0, 1) > 0.5)
            {
                1 => resolve;
            }
            
        }
        
        next_pitch => pitch;

        <<< "Voice pitch", pitch >>>;
    }
}

class Tenor extends Voice
{
    
    NRev reverb;
    
    ModalBar ugen => reverb => dac;
    1 => ugen.preset;
    
    VoicForm voice_ugen => dac;
    "ahh" => voice_ugen.phoneme;
    
    SetGain(0.0);
    
    fun void SetFrequency(float freq)
    {
        freq => ugen.freq;
        0.6 => ugen.strike;

        freq / 2 => voice_ugen.freq;
        
        SetGain(1.0);
    }

    fun void SetGain(float gain)
    {
        gain => ugen.gain;
        gain => voice_ugen.gain;
    }
}

class Soprano extends Voice
{    
    NRev reverb;
    
    ModalBar ugen => reverb => dac;
    0 => ugen.preset;
    
    //Saxofony sax => dac;
    
    VoicForm voic => dac;
    "aaa" => voic.phoneme;
    
    SetGain(0.0);
    
    fun void SetFrequency(float freq)
    {
        freq => ugen.freq;
        1.0 => ugen.strike;
        
        freq => voic.freq;
        
        SetGain(1.0);
    }
    
    fun void SetGain(float gain)
    {
        gain => ugen.gain;
        gain => voic.gain;
    }
}

class Bass extends Voice
{    
    NRev reverb;
    
    ModalBar ugen => reverb => dac;
    2 => ugen.preset;
    
    //Saxofony sax => dac;
    
    VoicForm voic => dac;
    "ooo" => voic.phoneme;
    
    SetGain(0.0);
    
    fun void SetFrequency(float freq)
    {
        freq => ugen.freq;
        1.0 => ugen.strike;
        
        freq / 4 => voic.freq;
        
        SetGain(1.0);
    }
    
    fun void SetGain(float gain)
    {
        gain => ugen.gain;
        gain => voic.gain;
    }
}

Voice @ voices[3];

Soprano v1 @=> voices[0];
Tenor v2 @=> voices[1];
Bass v3 @=> voices[2];

OrderVoices(voices);

<<< v1.order >>>;
<<< v2.order >>>;
<<< v3.order >>>;

v1.AddOtherVoice(v2);
v1.AddOtherVoice(v3);
v2.AddOtherVoice(v1);
v2.AddOtherVoice(v3);
v3.AddOtherVoice(v1);
v3.AddOtherVoice(v2);

spork ~ v1.Play() @=> Shred @ v1_shred;
spork ~ v2.Play() @=> Shred @ v2_shred;
spork ~ v3.Play() @=> Shred @ v3_shred;

while (!v1.Done() && !v2.Done() && !v3.Done())
{
    1::second => now;
}