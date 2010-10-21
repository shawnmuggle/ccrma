// Mike Rotondo
// 220A HW3

// play a cycle of pitches
5 => int nPitches;
// array to hold midi pitches (key numbers)
int keyn[nPitches]; 
[ 60, 62, 64, 65, 67] @=> keyn;

////////////////////////////////////////////////////////////////
// against a cycle of a different length varying instrument parameters
4 => int nInsts;
// arrays to hold loudnesses, pitch register transposition, channel, instrument type
float           loud[nInsts]; 
float           tran[nInsts]; 
int             chan[nInsts]; 
dur           durations[nInsts];
StkInstrument   inst[nInsts];
ADSR        envs[nInsts];

time nextNoteOffs[nInsts];

// loudness contour
[ 0.4, 0.4, 0.4, 0.4 ] @=> loud; 
// pitch register contour
[ 1.0, 1.0, 1.0, 1.0 ] @=> tran; // e.g., 2.0 for octave higher, 4.0 for two octaves, etc.
// channel contour 
[ 0, 1, 2, 3 ] @=> chan; // 0 1 2 3 in 4-channel mode
// strike position contour 
//[ 0.0, 0.1, 1.0 ] @=> float vent[]; // per-instrument amount, 0 - 1

[100::ms, 200::ms, 400::ms, 600::ms] @=> durations;
[now + durations[0], now + durations[1], now + durations[2], now + durations[3]] @=> nextNoteOffs;


// StkInstrument physical models to choose from
//     BandedWG BlowBotl BlowHole Bowed Brass Clarinet 
//     Flute StifKarp ModalBar Saxofony

// instantiate 3 instruments and set their output channels
Bowed x @=> inst[0] => LPF l1 => ADSR e1 => JCRev r1 => dac.chan(chan[0]);
Bowed y @=> inst[1] => LPF l2 => ADSR e2 => JCRev r2 => dac.chan(chan[1]);
Bowed z @=> inst[2] => LPF l3 => ADSR e3 => JCRev r3 => dac.chan(chan[2]);
Bowed zz @=> inst[3] => LPF l4 => ADSR e4 => JCRev r4 => dac.chan(chan[3]);

100 => l1.freq;
200 => l2.freq;
300 => l3.freq;
400 => l4.freq;

10::ms => e1.attackTime;
10::ms => e1.decayTime;
0.4 => e1.sustainLevel;
80::ms => e1.releaseTime;
10::ms => e2.attackTime;
10::ms => e2.decayTime;
0.4 => e2.sustainLevel;
80::ms => e2.releaseTime;
10::ms => e3.attackTime;
10::ms => e3.decayTime;
0.4 => e3.sustainLevel;
80::ms => e3.releaseTime;
10::ms => e4.attackTime;
10::ms => e4.decayTime;
0.4 => e4.sustainLevel;
80::ms => e4.releaseTime;

[e1, e2, e3, e4] @=> envs;

//for (0=>int i; i<nInsts; i++) inst[i].controlChange( 16, 0); // select wood block

////////////////////////////////////////////////////////////////
// global parameters
10.0 => dac.gain;                   // bump up the output level
1000::ms => dur ioi;                // starting inter-onset interval (inverse of tempo)
10::ms => dur minIoi;               // accelerate to this smallest ioi
0 => int p;                         // which pitch is next
0 => int i;                         // which instrument is next

////////////////////////////////////////////////////////////////
// sectionI
// when to change
10::second + now => time endOfSectionI;
while( now < endOfSectionI )
{
    for( 0 => int i; i < nInsts ; i++ )
    {
        if (now > nextNoteOffs[i])
        {
            //<<< "Starting note ", inst[i], " at pitch ", keyn[p] >>>;
            
            // STOP NOTE
            0.1 => inst[i].noteOff;
            1 => envs[i].keyOff;
            
            // START NOTE
            Std.mtof( keyn[p] ) * tran[i] => inst[i].freq; // assign pitch as freq
            1 +=> p;
            nPitches %=> p;
            
            loud[i] => inst[i].noteOn;      // start note
            1 => envs[i].keyOn;
            now + durations[i] => nextNoteOffs[i];
            durations[i] * 0.99 => durations[i];            
        }
        inst[i].freq() * .9999 => inst[i].freq;
    }
    1::ms => now;
}

2::second + now => time endOfInterlude;
0 => int counter;
while( now < endOfInterlude )
{
    for( 0 => int i; i < nInsts ; i++ )
    {
        inst[i].freq() + (i - 1.5) * counter * 0.000001 => inst[i].freq;
    }
    1::ms => now;
    1 +=> counter;
}


for( 0 => int i; i < nInsts ; i++ )
{
    // STOP NOTE
    //1 => inst[i].noteOff;
    1 => envs[i].keyOff;
}

////////////////////////////////////////////////////////////////
// sectionII
Clarinet x2 @=> inst[0] => e1 => dac.chan(chan[0]);
Clarinet y2 @=> inst[1] => e2 => dac.chan(chan[1]);
Clarinet z2 @=> inst[2] => e3 => dac.chan(chan[2]);
Clarinet zz2 @=> inst[3] => e4 => dac.chan(chan[3]);

200::ms => e1.attackTime;
0::ms => e1.decayTime;
0.4 => e1.sustainLevel;
80::ms => e1.releaseTime;
200::ms => e2.attackTime;
0::ms => e2.decayTime;
0.4 => e2.sustainLevel;
80::ms => e2.releaseTime;
200::ms => e3.attackTime;
0::ms => e3.decayTime;
0.4 => e3.sustainLevel;
80::ms => e3.releaseTime;
200::ms => e4.attackTime;
0::ms => e4.decayTime;
0.4 => e4.sustainLevel;
80::ms => e4.releaseTime;

[ 0.1, 0.1, 0.1, 0.1 ] @=> loud; 

[200::ms, 200::ms, 200::ms, 600::ms] @=> durations;
[now + durations[0], now + durations[1], now + durations[2], now + durations[3]] @=> nextNoteOffs;

10::second + now => time endOfSectionII;
0 => i;
0 => p;
while( now < endOfSectionII )
{
    // STOP NOTE
    0.1 => inst[i].noteOff;
    1 => envs[i].keyOff;
    
    // START NOTE
    Std.mtof( keyn[p] ) * tran[i] => inst[i].freq; // assign pitch as freq
    1 +=> p;
    nPitches %=> p;
    1 +=> i;
    nInsts %=> i;
    
    loud[i] => inst[i].noteOn;      // start note
    1 => envs[i].keyOn;
    now + durations[i] => nextNoteOffs[i];
    durations[i] * 0.99 => durations[i];            
    durations[i] + now => now;
}
for( 0 => int i; i < nInsts ; i++ )
{
    // STOP NOTE
    //1 => inst[i].noteOff;
    1 => envs[i].keyOff;
}
4::second => now;