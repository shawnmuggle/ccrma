% Mike Rotondo / Lab3-Q2

amps = [0.99 0.7 0.5 0];
durs = [0.3 0.7 3 1];
fs = 200;
[linenv, logenv] = adsr(amps, durs, fs);
plot(linenv);

durs = [0.1 0.1 1 3];
amps = [0.99 0.2 0.9 0];
fs = 44100;
[linenv, logenv] = adsr(amps, durs, fs);
num_partials = [1:10];
fund = 100;
f = fund * n;
tone = additive(f, rand(1, 10), fs, 4.2, 'tone');

size(tone)
size(logenv)
enved_tone = logenv' .* tone(1:end-1);
scaled_tone = 0.9 * enved_tone / max(enved_tone);
plot(scaled_tone);
sound(scaled_tone, fs);