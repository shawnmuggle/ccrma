SndBuf snd0 => dac.chan(0); // connect a sndBuf to 1st speaker
snd0 => dac.chan(1);
snd0 => dac.chan(2);
snd0 => dac.chan(3);

0 => dac.chan(0).gain;
0 => dac.chan(1).gain;
0 => dac.chan(2).gain;
0 => dac.chan(3).gain;

"/user/m/mrotondo/class/220a/hw2/slates.wav" => snd0.read; // play file from beginning

0 => int i;
while (true) // loop the following block
{
  dac.chan(i).gain(1.0);
  48000::samp => now;
  dac.chan(i).gain(0.0);
  i++;
}