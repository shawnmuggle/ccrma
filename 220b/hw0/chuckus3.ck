SinOsc s11 => SinOsc s21 => SinOsc s31 => SinOsc s41 => SinOsc s51 => SinOsc s61 => Gen17 g171 => LPF f1 => Gain g1 => g1 => dac;
SinOsc s12 => SinOsc s22 => SinOsc s32 => SinOsc s42 => SinOsc s52 => SinOsc s62 => Gen17 g172 => LPF f2 => Gain g2 => g2 => dac;

[1., 1.5, 0.25, 0.125, 0.06] => g171.coefs;
400 => f1.freq;

.002 => s11.freq;
s21.sync(1);
s31.sync(1);
s41.sync(1);
s51.sync(1);
s61.sync(1);


[0.125, 0.06, 0.03, 0.015] => g172.coefs;
600 => f2.freq;

0.01 => s12.freq;
s22.sync(1);
s32.sync(1);
s42.sync(1);
s52.sync(1);
s62.sync(1);

while(true)
{
    ms=>now;
}