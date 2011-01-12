PulseOsc p=>dac; [1.0,1.2,1.3,1.6,1.9]@=>float ps[]; 20=>int f; 1=>int k;
0=>int i; while(true){ f*k*ps[i++]=>p.freq; 0=>int j; while(j<ps.cap()){
p.freq()*ps[j++]=>p.freq; 100::ms=>now;} if(i>=ps.cap()){ 0=>i; k++;}}