SndBuf snd0 => Binaural.pssp[0]; // connect a sndBuf to 1st speaker
snd0 => Binaural.pssp[1];
snd0 => Binaural.pssp[2];
snd0 => Binaural.pssp[3];

0 => Binaural.pssp[0].gain;
0 => Binaural.pssp[1].gain;
0 => Binaural.pssp[2].gain;
0 => Binaural.pssp[3].gain;

"/user/m/mrotondo/class/220a/hw2/slates.wav" => snd0.read; // play file from beginning

Binaural.pssp[0].gain(1.0);
70287::samp => now;
Binaural.pssp[0].gain(0.0);
  
Binaural.pssp[1].gain(1.0);
66000::samp => now;
Binaural.pssp[1].gain(0.0);
  
Binaural.pssp[2].gain(1.0);
54000::samp => now;
Binaural.pssp[2].gain(0.0);
  
Binaural.pssp[3].gain(1.0);
36000::samp => now;
Binaural.pssp[3].gain(0.0);