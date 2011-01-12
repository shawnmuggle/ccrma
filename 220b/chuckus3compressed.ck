1=>int i;
while(i<3)
{
SinOsc ss[6];
0=>int z;
while(z<5)
{
    ss[z]=>ss[z+1];
    ss[z+1].sync(1);
    z++;
}
ss[5]=>Gen17 g=>Gain j=>dac;
i++*.001=>ss[0].freq;
[1.0]=>g.coefs;
}
day=>now;