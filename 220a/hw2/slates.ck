SndBuf snd0 => dac.chan(0); // connect a sndBuf to 1st speaker
snd0 => dac.chan(1);
snd0 => dac.chan(2);
snd0 => dac.chan(3);

0 => dac.chan(0).gain;
0 => dac.chan(1).gain;
0 => dac.chan(2).gain;
0 => dac.chan(3).gain;

"/user/m/mrotondo/class/220a/hw2/slates.wav" => snd0.read; // play file from beginning

dac.chan(0).gain(1.0);
70287::samp => now;
dac.chan(0).gain(0.0);
  
dac.chan(1).gain(1.0);
66000::samp => now;
dac.chan(1).gain(0.0);
  
dac.chan(2).gain(1.0);
54000::samp => now;
dac.chan(2).gain(0.0);
  
dac.chan(3).gain(1.0);
36000::samp => now;
dac.chan(3).gain(0.0);