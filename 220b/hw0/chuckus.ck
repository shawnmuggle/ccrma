Impulse i => LPF f => dac; 

20 => f.Q; 

SinOsc lfo => blackhole; 
27 => lfo.freq; 
800 => lfo.gain; 

while (ms=>now) 
{ 
    10 => i.next;
    lfo.last() + 300 => f.freq;
}