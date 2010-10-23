// Template for auditory streaming homework
// try me, I don't break into multiple streams... why?

// answer: the streaming illusion demonstrates grouping by similarity
// here there is only one group of sounds and therefore only one stream

////////////////////////////////////////////////////////////////
// play a cycle of pitches
4 => int nPitches;
// array to hold midi pitches (key numbers)
int keyn[nPitches]; 
[ 60, 62, 64, 66 ] @=> keyn;

////////////////////////////////////////////////////////////////
// against a cycle of a different length varying instrument parameters
3 => int nInsts;
// arrays to hold loudnesses, pitch register transposition, channel, instrument type
float           loud[nInsts]; 
float           tran[nInsts]; 
int             chan[nInsts]; 
BlowHole   inst[nInsts];

// loudness contour
//[ 0.1, 0.1, 0.1 ] @=> loud; 
[ 0.7, 0.7, 0.7 ] @=> loud; // set to this loudness
// pitch register contour
[ 1.0, 1.0, 1.0 ] @=> tran; // e.g., 2.0 for octave higher, 4.0 for two octaves, etc.
// channel contour 
[ 0, 0, 0 ] @=> chan;

[ 0.0, 0.1, 1.0 ] @=> float vent[]; // per-instrument amount, 0.0 ? 1.0
[ 1.0, 1.0, 127.0 ] @=> float reed[]; // per-instrument amount, 0.0 ? 127.0

// StkInstrument physical models to choose from
//     BandedWG BlowBotl Bowed Brass Clarinet 
//     Flute StifKarp ModalBar Saxofony

// instantiate 3 instruments and set their output channels
BlowHole x @=> inst[0] => dac.chan(chan[0]);
BlowHole y @=> inst[1] => dac.chan(chan[1]);
BlowHole z @=> inst[2] => dac.chan(chan[2]);

//0.2 => z.gain;

////////////////////////////////////////////////////////////////
// global parameters
100::ms => dur duration;            // set a common note duration
50.0 => dac.gain;                   // bump up the output level
1000::ms => dur ioi;                // starting inter-onset interval (inverse of tempo)
50::ms => dur minIoi;               // accelerate to this smallest ioi
0 => int p;                         // which pitch is next
0 => int i;                         // which instrument is next

// infinite time-loop engine
while( true )
{   
    <<< p, i >>>;                   // print pitch index, instrument index
    Std.mtof( keyn[p] ) * tran[i] => inst[i].freq; // assign pitch as freq
    inst[i].controlChange( 2, reed[i] ); // reed stiffness
     inst[i].vent( vent[i] ); // tonehole
    loud[i] => inst[i].noteOn;      // start note
    duration => now;                // wait
    0.1 => inst[i].noteOff;         // stop note
    p++; i++;                       // increment note and instrument
    (p % nPitches) => p;            // cycle pitch through full array
    (i % nInsts) => i;              // cycle instrument through full array

    // advance time by interval and calculate the next time interval
    ioi => now;
    ioi * 0.9 => ioi;               // accelerate 
    if (ioi < minIoi) minIoi => ioi;// can't go faster than minIoi 
}