[1, 1, 1] @=> int keep_playing[];
0 => int intro;
1 => int bridge;
2 => int ending;

fun void loadFile(string filename, SndBuf buf)
{
    // sound file
    "/Users/mrotondo/Music/Golden Gate Park July 2010/" => string snd_dir;
    snd_dir + filename => string full_filename;
    if( me.args() ) me.arg(0) => full_filename;
    
    // load the file
    //SndBuf buf;
    full_filename => buf.read;
}

fun void playSound(SndBuf buf, int num, float rate, float gain, int start_samp, dur duration, float prob)
{
	if (Std.rand2f(0.0, 1.0) > prob) return;
    buf => Gain g => dac;
    gain => g.gain;
    rate => buf.rate;
    start_samp => buf.pos;
	while (num > 0)
	{
		duration => now;
		start_samp => buf.pos;
		num - 1 => num;
	}
}

fun void loopSound(SndBuf buf, 
                   dur playtime,
                   dur interval, 
                   float pan, 
                   float max_chorus_mix,
                   float min_q, // never go below 1 ?!
                   float max_q,
                   dur fade_in_duration
                   )
{
    // the patch
    buf => DelayA d => ResonZ r => d => Chorus c => Envelope e => dac;
    5::second => d.max;
    
    fade_in_duration => e.duration;
    0.0 => e.value;
    1 => e.keyOn;
    
    
    now + playtime => time end;
    // time loop
    while( now < end )
    {
        //0 => buf.pos;
        //Std.rand2f(.2,.9) => buf.gain;
        Std.rand2f(.5, 1.5) => buf.rate;
        buf.rate() * 1000 => c.modFreq;
        
        Std.rand2f(500, 2000) => r.freq;
        
        Std.rand2f(min_q, max_q) => r.Q; // lower q = more ringy
        r.Q() / 100.0 => c.modDepth;
        
        Std.rand2f(0, max_chorus_mix) => c.mix;
        
        Std.rand2f(10, 1000)::samp => d.delay;
        
        Std.rand2(0, buf.samples()) => buf.pos;
        //1.0 => buf.rate;
        if (interval < end - now) {
            interval => now;
        } else {
            end - now => now;
        }
    }
}

fun void stutterSound(SndBuf buf, int start_samp, dur baseInterval, float pan, float rate, int section)
{
    rate => buf.rate;
    start_samp => buf.pos;
    
    buf => Pan2 p => dac;
    
    pan => p.pan;
    
    while( keep_playing[section] )
    {
        buf.pos() - Std.rand2(0, 7000) => buf.pos;
        baseInterval => now;
        if (buf.pos() > buf.samples())
        {
            start_samp => buf.pos;
        }
    }
}

fun void playPercussiveSounds(SndBuf buf, int onsets[], dur playtime, dur interval, float gain, int section)
{
	buf => Gain g => dac;
	gain => g.gain;
	int sound_index;
	while (keep_playing[section])
	{
		1.0 => buf.rate;
		Std.rand2(0, onsets.cap() - 1) => sound_index;
		onsets[sound_index] => buf.pos;
		playtime => now;
		0.0 => buf.rate;
		interval - playtime => now;
	}
}

fun void slowlyHighpassSound(SndBuf buf, int startSamp, dur duration, float startFreq, float endFreq, float rate)
{
	rate => buf.rate;
	buf => HPF f => dac;
	startFreq => f.freq;
	now => time start;
	now + duration => time end;
	while (now - start < duration)
	{
		startFreq + (endFreq - startFreq) * ((now - start) / duration) => f.freq;
		1::ms => now;
	}
}

fun void playLowSoundSlice(string filename, int startSamp, dur duration, float rate)
{
    SndBuf buf;
    loadFile(filename, buf);
    rate => buf.rate;
    
    buf => DelayA d => BPF bf => d => ADSR env => dac;
    280 => bf.freq;
    1 => bf.Q;
    0.9 => bf.gain;
    10::ms => d.delay;
    //Std.rand2f(1, 3) => lf.Q;
    
    duration / 3.0 => env.releaseTime;
    
    1 => env.keyOn;
    startSamp => buf.pos;
    duration / 2.0 => now;
    1 => env.keyOff;
    duration / 2.0 => now;
}

SndBuf water1;
loadFile("LS100034.WAV", water1);
SndBuf water2;
loadFile("LS100035.WAV", water2);
SndBuf water3;
loadFile("LS100090.WAV", water3);

<<< "HEY-1" >>>;
SndBuf woodrub1;
loadFile("LS100041.WAV", woodrub1);

<<< "HEY0" >>>;
SndBuf clang1;
loadFile("LS100076.WAV", clang1);
SndBuf clang2;
loadFile("LS100077.WAV", clang2);
SndBuf clang3;
loadFile("LS100078.WAV", clang3);

<<< "HEY1" >>>;
SndBuf splash;
loadFile("LS100088.WAV", splash);

<<< "HEY2" >>>;

SndBuf fence1;
loadFile("LS100006.WAV", fence1);
SndBuf fence2;
loadFile("LS100007.WAV", fence2);
<<< "HEY4" >>>;

SndBuf clang4;
loadFile("LS100010.WAV", clang4);
SndBuf clang5;
loadFile("LS100013.WAV", clang5);

<<< "HEY5" >>>;

SndBuf friction1;
loadFile("LS100038.WAV", friction1);
SndBuf friction2;
loadFile("LS100039.WAV", friction2);

SndBuf whacks1;
loadFile("LS100033.WAV", whacks1);
SndBuf whacks1_2;
loadFile("LS100033.WAV", whacks1_2);
SndBuf whacks1_3;
loadFile("LS100033.WAV", whacks1_3);

SndBuf sticks1;
loadFile("LS100032.WAV", sticks1);
[120545, 175220, 181254, 189874, 202804, 215303, 325910, 358235, 375044, 412110, 485197, 610618, 635616, 707342, 766768, 788318, 834775, 867065, 948864 ] @=> int onsets[];

SndBuf hiss;
loadFile("LS100074.WAV", hiss);
SndBuf machine;
loadFile("LS100075.WAV", machine);

SndBuf beeps;
loadFile("LS100065.WAV", beeps);

SndBuf smash1;
loadFile("LS100073.WAV", smash1);
SndBuf smash2;
loadFile("LS100073.WAV", smash2);
SndBuf smash3;
loadFile("LS100073.WAV", smash3);

SndBuf squeaks1;
loadFile("LS100042.WAV", squeaks1);

SndBuf branches1;
loadFile("LS100093.WAV", branches1);

SndBuf smacks1;
loadFile("LS100017.WAV", smacks1);
[23350, 105806, 180600, 244082, 299538, 337847, 385642] @=> int smackOnsets[];
SndBuf glass1;
loadFile("LS100018.WAV", glass1);
[6523, 75020, 180481, 343569, 405540, 517516, 592545, 652343, 812167, 1016570, 1148124] @=> int glassOnsets[];

SndBuf leaves;
loadFile("LS100058.WAV", leaves);

SndBuf can;
loadFile("LS100063.WAV", can);

SndBuf clang;
loadFile("LS100096.WAV", clang);

SndBuf reverbStuff;
loadFile("LS100097.WAV", reverbStuff);

spork ~ loopSound(water1, 20::second, 800::ms, -0.5, 0, 100, 200, 8::second);
spork ~ loopSound(water2, 20::second, 1200::ms, 0.5, 0, 100, 200, 10::second);

4::second => now;

spork ~ playSound(woodrub1, 1, 0.3, 0.1, 38593, 10::second, 1.0);

8::second => now;

spork ~ stutterSound(clang1, 533735, 200::ms, 0.5, 1.0, intro);
spork ~ stutterSound(clang2, 53997,  100::ms, -0.5, 1.0, intro);
spork ~ stutterSound(clang3, 56429, 300::ms, 1.0, 1.0, intro);

2::second => now;

spork ~ playSound(splash, 1, 1.0, 2.0, 551152, 71278::samp, 1.0);

1::second => now;

spork ~ loopSound(fence1, 20::second, 500::ms, -0.5, 1, 10, 100, 1::second);
100::ms => now;
spork ~ loopSound(fence2, 20::second, 750::ms, 0.5, 1, 10, 100, 3::second);
100::ms => now;
spork ~ stutterSound(clang4, 0, 100::ms, -1, 1.0, intro);
100::ms => now;
spork ~ stutterSound(clang5, 0, 100::ms, 1, 1.0, intro);

20::second => now;

spork ~ stutterSound(friction2, 130000, 500::ms, 1, 0.7, intro);
spork ~ stutterSound(friction1, 147215, 200::ms, -1, 0.7, intro);

10::second => now;

spork ~ playSound(whacks1, 2, 0.5, 0.6, 77055, (321181 * (1 / 0.5))::samp, 1.0);
200::ms => now;
spork ~ playSound(whacks1_2, 2, 0.5, 0.6, 77055, (321181 * (1 / 0.5))::samp, 1.0);
100::ms => now;
spork ~ playSound(whacks1_3, 2, 0.5, 2, 77055, (321181 * (1 / 0.5))::samp, 1.0);
spork ~ playPercussiveSounds(sticks1, onsets, 6000::samp, .2::second, 1.5, intro);
(321181*4)::samp => now;

false => keep_playing[intro];

spork ~ playSound(hiss, 1, 1.0, 1.0, 0, 30::second, 1.0);
spork ~ playSound(machine, 1, 1.0, 1.0, 0, 30::second, 1.0);

5::second => now;

spork ~ playSound(beeps, 1, 1.0, 1.0, 0, 5::second, 1.0);

5::second => now;

spork ~ playSound(smash1, 1, 0.7, 1.5, 0, 5::second, 1.0);
100::ms => now;
spork ~ playSound(smash2, 1, 0.8, 1.7, 0, 5::second, 1.0);
200::ms => now;
spork ~ playSound(smash3, 1, 1.2, 1.3, 0, 5::second, 1.0);

4::second => now;

spork ~ slowlyHighpassSound(squeaks1, 0, 10::second, 0, 5000, 1.0);
spork ~ stutterSound(branches1, 0, 400::ms, -0.5, 1.0, bridge);
spork ~ playPercussiveSounds(smacks1, smackOnsets, 10000::samp, .4::second, 1.2, bridge);
spork ~ playPercussiveSounds(glass1, glassOnsets, 8000::samp, .2::second, 1.5, bridge);
spork ~ playSound(whacks1, 2, 0.5, 0.6, 77055, (321181 * (1 / 0.5))::samp, 1.0);
100::ms => now;
spork ~ playSound(whacks1_2, 2, 0.5, 0.6, 77055, (321181 * (1 / 0.5))::samp, 1.0);
300::ms => now;
spork ~ playSound(whacks1_3, 2, 0.5, 2, 77055, (321181 * (1 / 0.5))::samp, 1.0);

spork ~ stutterSound(leaves, 157000, 600::ms, 0.5, 1.0, bridge);

10::second => now;

spork ~ stutterSound(water3, 39603, 100::ms, -1.0, 0.6, bridge);
spork ~ stutterSound(can, 0, 300::ms, 1.0, 1.3, bridge);

10::second => now;

spork ~ playSound(clang, 1, 1.0, 1.0, 0, 10::second, 1.0);

4::second => now;

spork ~ loopSound(reverbStuff, 20::second, 500::ms, 0.0, 0.4, 50, 100, 5::second);

// from intro
spork ~ loopSound(fence1, 20::second, 500::ms, -0.5, 1, 10, 100, 1::second);
spork ~ loopSound(fence2, 20::second, 750::ms, 0.5, 1, 10, 100, 3::second);
spork ~ stutterSound(clang4, 0, 100::ms, -1, 1.0, bridge);
spork ~ stutterSound(clang5, 0, 100::ms, 1, 1.0, bridge);

10::second => now;

false => keep_playing[bridge];

spork ~ stutterSound(clang1, 533735, 200::ms, 0.5, 1.0, ending);
spork ~ stutterSound(clang2, 53997,  100::ms, -0.5, 1.0, ending);
spork ~ stutterSound(clang3, 56429, 300::ms, 1.0, 1.0, ending);

30::second => now;
