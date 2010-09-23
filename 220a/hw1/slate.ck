fun void beep( UGen gen, float gain, float seconds )
{
    gain => gen.gain;
    seconds::second => now;
    0 => gen.gain;
    0.1::second => now;
}

220 => int base_freq;
4 => int channels;
SinOsc oscs[channels];

for ( 0 => int i; i < channels; i++ ) {
    0.0 => oscs[i].gain;
    base_freq * (i + 1) => oscs[i].freq;
    oscs[i] => dac.chan(i);
    for ( 0 => int j; j <= i; j++ ) {
        beep(oscs[i], 0.1, 0.2);
    }
}

for ( 0 => int i; i < channels; i++ ) {
    0.1 => oscs[i].gain;
}

for ( 0 => int i; i < 4000; i++ ) {
    for ( 0 => int j; j < channels; j++ ) {
	oscs[j].freq() * 0.999 => oscs[j].freq;
	oscs[j].gain() * 0.999 => oscs[j].gain;
    }
    1::ms => now;
}